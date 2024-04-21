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
 * Containes queues that need to be freed in break;case of SIGINT
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
    // string_t s = "BYE\r\n";
    // msg_t m;
    // memset(&m, '\0', sizeof(m));
    // parse_tcp(s, &m);
    // return 0;


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
        int a;
        int next_timeout = get_next_timeout(msg_out_buf);
        next_timeout = next_timeout > POLL_INTERVAL ? POLL_INTERVAL : next_timeout;
        next_timeout = next_timeout == 0 ? POLL_INTERVAL : next_timeout;
        if ((a = poll(pollfd.pollfd_list, WELCOME_SOCK_COUNT + pollfd.cnt, next_timeout)) > 0) {
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
                    process_msg_sock(pollfd.pollfd_list[i].fd);
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
    msg_t msg;
    int fd_accept = 0;
    int buf_size;

    memset(&msg, '\0', sizeof(msg));
    memset(&client_addr, '\0', sizeof(client_addr));

    // Create new file descriptor
    if (server_accept(e_udp, &fd_accept, NULL)) {
        return errno;
    }

    // Read first message of a client which will determine his address
    // buf_size = server_read_sock(e_udp, get_udp_socket(), &client_addr, &buf);
    buf_size = read_msg(e_udp, get_udp_socket(), &client_addr, &msg);
    if (buf_size == -1) {
        // TODO smth wrong
        return 0;
    }

    client = get_client_by_addr(clients, client_addr);
    if (client != NULL) {
        // Client already exists. Notify him about that
        message_content_t err_msg;
        memset(&err_msg, '\0', sizeof(err_msg));
        memcpy(&err_msg, "This address is currently in use.", 37);
        send_error(e_udp, fd_accept, client_addr, err_msg);
        return 0;
    }
    // Not exist ? create client and save fd
    if (add_poll_fd(&pollfd, fd_accept, e_udp)) {
        // If failed -> cannot get connection, error message should be sent to client
        // Accepted socket won't saved
        message_content_t err_msg;
        memset(&err_msg, '\0', sizeof(err_msg));
        memcpy(&err_msg, "Connection could not be established.", 37);
        send_error(e_udp, fd_accept, client_addr, err_msg);
        return 0;
    } // Fd added to pollfd list

    client = queue_create_item();
    if (client == NULL) {
        message_content_t err_msg;
        memset(&err_msg, '\0', sizeof(err_msg));
        memcpy(&err_msg, "Connection could not be established.", 37);
        send_error(e_udp, fd_accept, client_addr, err_msg);
        server_close_client(fd_accept);
        return errno;
    }
    client->data.client.sockfd = fd_accept;
    client->id = client_cnt++;
    client->data.client.protocol = e_udp;
    client->data.client.msg_count = 0;
    client->data.client.state = s_ac; // Accepted
    client->data.client.addr = client_addr;
    queue_add(clients, client);

    // FD and Address are saved -> read and process the message 
    return process_msg(client, msg);
}

int process_msg_sock(int fd) {
    queue_item_t * client = NULL;
    int buf_size = 0;
    msg_t msg;
    
    client = get_client_by_fd(clients, fd);
    if (client == NULL) {
        // Something went terribly wrong...
        // TODO send bye message to socket, close connection, delete fd
        return errno = error_out(error_fatl_client_missing, __LINE__, __FILE__, NULL); 
    }
    memset(&msg, '\0', sizeof(msg));
    buf_size = read_msg(client->data.client.protocol, client->data.client.sockfd, &client->data.client.addr, &msg);
    if (buf_size == -1) {
        // No message
        return 0;
    }
    process_msg(client, msg);

    return 0;
}

int process_msg(queue_item_t * client, msg_t msg_in) {
    msg_t msg_out;
    memset(&msg_out, '\0', sizeof(msg_out));
    // If UDP -> send confirm
    if (client->data.client.protocol == e_udp && msg_in.type != e_confirm) {
        msg_t conf;
        string_t resp;
        int resp_size;
        memset(&conf, '\0', sizeof(conf));
        conf.type = e_confirm;
        conf.data.confirm.ref_id = msg_in.id;
        compose(e_udp, conf, &resp, &resp_size);
        send_msg(client->data.client.protocol, client->data.client.sockfd, client->data.client.addr, conf);
    }
    if (execute_msg(client, msg_in, &msg_out)) {
        // If response should be sent
        send_msg(client->data.client.protocol, client->data.client.sockfd, client->data.client.addr, msg_out);
        // TODO If udp -> add to queue
    }
    return 0;
}

bool execute_msg(queue_item_t * client, msg_t msg_in, msg_t * msg_out) {
    switch (msg_in.type) {
        case e_confirm:
            queue_item_t * item = queue_get(msg_out_buf, msg_in.data.confirm.ref_id);
            if (item != NULL) {
                item->data.msg.is_confirmed = true;
            }
            return false;
        case e_reply:
            msg_out->id = msg_cnt++;
            msg_out->is_confirmed = false;
            msg_out->retry_count = 0;
            msg_out->timeout = argv.udp_timeout;
            msg_out->type = e_err;
            memcpy(&(msg_out->data.err.display_name), "Server", 6);
            memcpy(&(msg_out->data.err.content), "Reply cannot be sent by client.", 32);
            return true;
        case e_auth:
            msg_out->id = msg_cnt++;
            msg_out->is_confirmed = false;
            msg_out->retry_count = 0;
            msg_out->timeout = argv.udp_timeout;
            msg_out->type = e_reply;
            msg_out->data.reply.ref_id = msg_in.id;
            msg_out->data.reply.result = true;
            memcpy(&(client->data.client.username), msg_in.data.auth.username, sizeof(msg_in.data.auth.username));
            memcpy(&(client->data.client.display_name), msg_in.data.auth.display_name, sizeof(msg_in.data.auth.display_name));
            memcpy(&(client->data.client.channel_id), "default", 8);
            memcpy(&(msg_out->data.reply.content), "Joined default channel.", 24);
            return true;
        case e_join:
            msg_out->id = msg_cnt++;
            msg_out->is_confirmed = false;
            msg_out->retry_count = 0;
            msg_out->timeout = argv.udp_timeout;
            msg_out->type = e_reply;
            msg_out->data.reply.ref_id = msg_in.id;
            msg_out->data.reply.result = true;
            memcpy(&(client->data.client.channel_id), msg_in.data.join.channel_id, sizeof(msg_in.data.join.channel_id));
            memcpy(&(client->data.client.display_name), msg_in.data.join.display_name, sizeof(msg_in.data.join.display_name));
            memcpy(&(msg_out->data.reply.content), "Channel was joined.", 20);
            return true;
        case e_msg:
            *msg_out = msg_in;
            msg_out->id = msg_cnt++;
            msg_out->is_confirmed = false;
            msg_out->retry_count = 0;
            msg_out->timeout = argv.udp_timeout;
            msg_out->type = e_msg;
            memcpy(client->data.client.display_name, msg_in.data.msg.display_name, sizeof(display_name_t)); 
            forward_msg_channel(client, *msg_out, client->data.client.channel_id);
            return false;
        case e_err: break;
        case e_bye: break;
    }
    return false;
}

int forward_msg_all(queue_item_t * client, msg_t msg) {
    queue_item_t * item;

    item = queue_first(clients);
    while (item != NULL) {
        if (cmp_clients(client, item)) {
            // Skip message sender
            continue;
        }
        if (item->data.client.protocol == e_tcp) {
            send_msg(item->data.client.protocol, item->data.client.sockfd, item->data.client.addr, msg);
        } else {
            send_msg(item->data.client.protocol, item->data.client.sockfd, item->data.client.addr, msg);
            // TODO add msg to queue
        }
        item = queue_next(item);
    }

    return 0;
}

int forward_msg_channel(queue_item_t * client, msg_t msg, channel_id_t channel) {
    queue_item_t * item;
    
    item = queue_first(clients);
    while (item != NULL) {
        item = queue_next(item);
    }

    item = queue_first(clients);
    while (item != NULL) {
        if (cmp_clients(client, item) || strcmp(item->data.client.channel_id, channel)) {
            // Skip if message sender or client with another channel
            item = queue_next(item);
            continue;
        }
        
        if (item->data.client.protocol == e_tcp) {
            send_msg(item->data.client.protocol, item->data.client.sockfd, item->data.client.addr, msg);
        } else {
            send_msg(item->data.client.protocol, item->data.client.sockfd, item->data.client.addr, msg);
            // TODO add msg to queue
        }
        item = queue_next(item);
    }
    
    return 0;
}

void send_error(transport_protocol_t protocol, int sockfd, struct sockaddr_in addr, message_content_t err_msg) {
    queue_item_t * msg = queue_create_item();
    msg->id = msg_cnt++;
    msg->data.msg.id = msg_cnt;
    msg->type = e_item_msg;
    msg->data.msg.is_confirmed = false;
    msg->data.msg.retry_count = 0;
    msg->data.msg.timeout = argv.udp_timeout;
    msg->data.msg.type = e_err;
    memcpy(&(msg->data.msg.data.err.display_name), "Server", 6);
    memcpy(&(msg->data.msg.data.err.content), err_msg, sizeof(*err_msg));
    send_msg(protocol, sockfd, addr, msg->data.msg);
    if (protocol == e_udp) {
        queue_add(msg_out_buf, msg);
    } else {
        queue_destroy_item(msg);
    }
}

void send_msg(transport_protocol_t protocol, int sockfd, struct sockaddr_in addr, msg_t msg) {
    string_t buf;
    int buf_size = 0;
    compose(protocol, msg, &buf, &buf_size);
    server_send(protocol, sockfd, addr, buf, buf_size);
    fprintf(stdout, "SENT %s:%d | ", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
    print_msg_type(msg.type);
    fprintf(stdout, "\n");
}

int read_msg(transport_protocol_t protocol, int sockfd, struct sockaddr_in * addr, msg_t * msg) {
    string_t buf;
    int buf_size = 0;
    buf_size = server_read_sock(protocol, sockfd, addr, &buf);
    if (buf_size != -1) {
        if (parse(protocol, buf, buf_size, msg)) {
            // Parse failed
            message_content_t err_msg;
            memset(&err_msg, '\0', sizeof(err_msg));
            memcpy(&err_msg, "Received message contains error.", 33);
            send_error(protocol, sockfd, *addr, err_msg);
        }
        fprintf(stdout, "RECV %s:%d | ", inet_ntoa(addr->sin_addr), ntohs(addr->sin_port));
        print_msg_type(msg->type);
        fprintf(stdout, "\n");
    }
    return buf_size;
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