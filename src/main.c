/**
 * @file main.c
 * @author Sniehovskyi Nikita (xsnieh00)
 * @date 11.04.2024
 * @brief Root program logic.
 */

#include "main.h"

/**
 * @brief List of all connected clients.
 * client.sockfd corresponds to pollfd+WELCOME_SOCK_COUNT
 * IF client.sockfd is 0 -> client does not exist
 * Containes queues that need to be freed in case of SIGINT
 */
queue_t * clients;

/**
 * @brief Buffer for out messages (UDP msg confirmation)
 * Cleared when confirmation is received.
 * If confirmation was not received -> client is closed, messages are erased 
 */
queue_t * msg_out_buf;

/**
 * @brief List of all fds which should be polled
 * Might be gaps in records if one of the clients quited
 * If fd is 0 -> socket is unused
 */
pollfd_list_t pollfd;

/**
 * @brief Client counter 
 */
uint32_t client_cnt = 0;

/**
 * @brief Message counter 
 */
uint32_t msg_cnt = 0;

int main(int argc, char **argv_in) {
    clients = NULL;
    msg_out_buf = NULL;
    signal(SIGINT, sigintHandler);
    parse_argv(argc, argv_in, &argv);

    if (argv.is_help) {
        printf("%s\n", help_msg);
        return 0;
    }

    pollfd.pollfd_list = malloc(sizeof(struct pollfd) * POLL_REALLOC_DELTA);
    pollfd.protocol = malloc(sizeof(transport_protocol_t) * POLL_REALLOC_DELTA);
    memset(pollfd.pollfd_list, '\0', sizeof(struct pollfd) * POLL_REALLOC_DELTA);
    pollfd.size = POLL_REALLOC_DELTA;
    pollfd.cnt = 0;
    if (pollfd.pollfd_list == NULL) {
        stop(error_out(error_memo_mem_alloc_fail, __LINE__, __FILE__, NULL));
    }

    clients = queue_create();
    if (clients == NULL) {
        stop(errno);
    }
    msg_out_buf = queue_create();
    if (msg_out_buf == NULL) {
        stop(errno);
    }

    if (server_open()) {
        stop(errno);
    }
    
    pollfd.pollfd_list[0].fd = get_tcp_socket();
    pollfd.protocol[0] = e_tcp;
    pollfd.pollfd_list[0].events = POLLIN;
    pollfd.pollfd_list[1].fd = get_udp_socket();
    pollfd.protocol[1] = e_udp;
    pollfd.pollfd_list[1].events = POLLIN;

    while (true) {
        int next_timeout;
        next_timeout = get_next_timeout(msg_out_buf);
        next_timeout = next_timeout > POLL_INTERVAL ? POLL_INTERVAL : next_timeout;
        if (poll(pollfd.pollfd_list, WELCOME_SOCK_COUNT + pollfd.cnt, next_timeout) > 0) {
            // TCP Welcome
            if (pollfd.pollfd_list[0].revents & POLLIN) {
                tcp_polling();
            }
            // UDP Welcome
            if (pollfd.pollfd_list[1].revents & POLLIN) {
                udp_polling();
            }
            for (int i = WELCOME_SOCK_COUNT; i < WELCOME_SOCK_COUNT + pollfd.cnt; i++) {
                if (pollfd.pollfd_list[i].fd != 0 && (pollfd.pollfd_list[i].revents & POLLIN)) {
                    process_msg(pollfd.pollfd_list[i].fd);
                }
            }
        }
    }

    stop(0);
}

int tcp_polling() {
    struct sockaddr_in client_addr;
    queue_item_t * client;
    int fd_accept = 0;

    if (server_accept(e_tcp, &fd_accept, &client_addr)) {
        return errno;
    }
    client = get_client_by_addr(clients, client_addr);
    if (client != NULL) {
        // Client already exists. Notify him about that
        // TODO send smth
        return 0;
    }
    // Not exist ? create client and save fd
    if (add_poll_fd(&pollfd, fd_accept, e_tcp)) {
        // If failed -> cannot get connection, error message should be sent to client
        // Accepted socket won't saved
        // TODO send reply/error to new_fd fd
        return 0;
    }
    // Fd added to pollfd list
    // TODO read welcome message and process it
    client = queue_create_item();
    if (client == NULL) {
        // TODO read/clear fd to not block process
        return errno;
    }
    client->data.client.sockfd = fd_accept;
    client->id = client_cnt++;
    client->data.client.protocol = e_tcp;
    client->data.client.addr = client_addr;
    client->data.client.msg_count = 0;
    queue_add(clients, client);

    return 0;
}

int udp_polling() {
    struct sockaddr_in client_addr;
    queue_item_t * client;
    int fd_accept = 0;

    if (server_accept(e_udp, &fd_accept, &client_addr)) {
        return errno;
    }
    client = get_client_by_addr(clients, client_addr);
    if (client != NULL) {
        // Client already exists. Notify him about that
        // TODO send smth
        return 0;
    }
    // Not exist ? create client and save fd
    if (add_poll_fd(&pollfd, fd_accept, e_udp)) {
        // If failed -> cannot get connection, error message should be sent to client
        // Accepted socket won't saved
        // TODO send reply/error to new_fd fd
        return 0;
    }
    // Fd added to pollfd list
    // TODO read welcome message and process it
    client = queue_create_item();
    if (client == NULL) {
        // TODO read/clear fd to not block process
        return errno;
    }
    client->data.client.sockfd = fd_accept;
    client->id = client_cnt++;
    client->data.client.protocol = e_udp;
    client->data.client.addr = client_addr;
    client->data.client.msg_count = 0;
    queue_add(clients, client);

    return 0;
}

int process_msg(int fd) {
    queue_item_t * client = NULL;
    string_t buf = "";
    int buf_size = 0;

    client = get_client_by_fd(clients, fd);
    if (client == NULL) {
        // Something went terribly wrong...
        // TODO send bye message to socket, close connection, delete fd
        return errno = error_out(error_fatl_client_missing, __LINE__, __FILE__, NULL); 
    }
    memset(buf, '\0', sizeof(buf));
    if (server_read(client, &buf, &buf_size)) {
        return errno;
    }
    if (buf_size == -1) {
        // If socket is nonblocking, -1 means nothing is available (man recv)
        return 0;
    }
    if (buf_size == 0) {
        // Close socket
        server_close_client(fd);
        // Destroy client
        queue_destroy_item(queue_remove(clients, client->id));
        // Delete fd from active fd list
        del_poll_fd(&pollfd, fd);
        // TODO send bye message mb?, notify others
        return 0;
    }
    // TODO parse msg
    // TODO execute msg
    send(fd, buf, buf_size, 0); // TODO.RM

    return 0;
}

void sigintHandler(int signal) {
    signal = signal; // TODO rm?
    stop(0);
}

void stop(int exit_code) {
    if (clients != NULL) {
        queue_destroy(clients);
    }
    if (msg_out_buf != NULL) {
        queue_destroy(msg_out_buf);
    }
    server_close();
    for (int i = WELCOME_SOCK_COUNT; i < pollfd.size; i++) {
        if (pollfd.pollfd_list[i].fd != 0) {
            close(pollfd.pollfd_list[i].fd);
        }
    }
    free(pollfd.pollfd_list);
    free(pollfd.protocol);
    pollfd.pollfd_list = NULL;
    exit(exit_code);
}