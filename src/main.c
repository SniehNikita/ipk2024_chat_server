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
        fprintf(stdout, "%s\n", help_msg);
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
        } else {
            // If nothing received within poll interval -> calculate timeout
            udp_timeout(next_timeout);
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
    client->data.client.state = s_ac; // Accept
    client->data.client.addr = client_addr;
    queue_add(clients, client);


    // FD and Address are saved -> read and process the message 
    return process_msg(client, msg);
}

int udp_timeout(int interval) {
    queue_item_t * item = queue_first(msg_out_buf);

    while (item != NULL) {
        item->data.msg.timeout -= interval;

        if (item->data.msg.timeout <= 0 && !item->data.msg.is_confirmed) {
            // If message timed out -> retry
            if (item->data.msg.retry_count <= argv.udp_retransmissions) {
                // If retransmission not exceeded
                send_msg(e_udp, item->data.msg.sockfd, item->data.msg.addr, item->data.msg, true);
            } else {
                // TODO close client
            }
            item->data.msg.retry_count++;
        }

        item = queue_next(item);
    }

    return 0;
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
        send_msg(client->data.client.protocol, client->data.client.sockfd, client->data.client.addr, conf, false);
    }
    // If client is in end state -> accept only confirms and bye, otherwise send error
    if (client->data.client.state == s_en && (msg_in.type != e_confirm || msg_in.type != e_bye)) {
        message_content_t err_msg;
        memset(&err_msg, '\0', sizeof(err_msg));
        memcpy(&err_msg, "Client was already closed.", 27);
        send_error(client->data.client.protocol, client->data.client.sockfd, client->data.client.addr, err_msg);
        return 0;
    }
    if (execute_msg(client, msg_in, &msg_out)) {
        // If response should be sent
        send_msg(client->data.client.protocol, client->data.client.sockfd, client->data.client.addr, msg_out, false);
    }
    if (msg_in.type == e_bye) {
        // In case BYE received client is already destroyed
        return 0;
    }
    if (client->data.client.state == s_er) {
        // If client is in error state
        client->data.client.state = s_en; // Assign end state
        close_client(client); // And close client
    }
    return 0;
}

bool execute_msg(queue_item_t * client, msg_t msg_in, msg_t * msg_out) {
    switch (msg_in.type) {
        case e_confirm:
            queue_item_t * item = queue_get(msg_out_buf, msg_in.data.confirm.ref_id);
            queue_item_t * t = queue_first(msg_out_buf);
            while (t != NULL) {
                t = queue_next(t);
            }
            if (item != NULL) {
                item->data.msg.is_confirmed = true;
                if (item->data.msg.type == e_bye) {
                    // If confirmed server bye message -> delete client. Confirmed message will be deleted with all user data
                    delete_client(client);
                } else {
                    // Else just destroy confirmed message
                    queue_destroy_item(queue_remove(msg_out_buf, msg_in.data.confirm.ref_id));
                }
            }
            return false;
        case e_reply:
            msg_out->type = e_err;
            memcpy(&(msg_out->data.err.display_name), "Server", 6);
            memcpy(&(msg_out->data.err.content), "Reply cannot be sent by client.", 32);
            client->data.client.state = s_er;
            return true;
        case e_auth:
            msg_out->type = e_reply;
            msg_out->data.reply.ref_id = msg_in.id;
            msg_out->data.reply.result = true;
            memcpy(&(client->data.client.username), msg_in.data.auth.username, sizeof(msg_in.data.auth.username));
            memcpy(&(client->data.client.display_name), msg_in.data.auth.display_name, sizeof(msg_in.data.auth.display_name));
            memcpy(&(client->data.client.channel_id), "default", 8);
            memcpy(&(msg_out->data.reply.content), "Joined default channel.", 24);
            notify_join(client); // Notify other clients about join
            client->data.client.state = s_op;
            return true;
        case e_join:
            notify_leave(client); // Leave before name change            
            msg_out->type = e_reply;
            msg_out->data.reply.ref_id = msg_in.id;
            msg_out->data.reply.result = true;
            memcpy(&(client->data.client.channel_id), msg_in.data.join.channel_id, sizeof(msg_in.data.join.channel_id));
            memcpy(&(client->data.client.display_name), msg_in.data.join.display_name, sizeof(msg_in.data.join.display_name));
            snprintf((char *)&(msg_out->data.reply.content), sizeof(msg_out->data.reply.content), "You joined %s channel.", msg_in.data.join.channel_id);
            notify_join(client); // Enter with new name
            client->data.client.state = s_op;
            return true;
        case e_msg:
            *msg_out = msg_in;
            msg_out->type = e_msg;
            memcpy(client->data.client.display_name, msg_in.data.msg.display_name, sizeof(display_name_t)); 
            forward_msg_channel(client, *msg_out, client->data.client.channel_id);
            client->data.client.state = s_op;
            return false;
        case e_err: break;
        case e_bye:
            notify_leave(client);            
            client->data.client.state = s_en;
            delete_client(client);
            return false;
    }
    return false;
}

int forward_msg_channel(queue_item_t * client, msg_t msg, channel_id_t channel) {
    queue_item_t * item;
    item = queue_first(clients);
    while (item != NULL) {
        if (item->data.client.state == s_en) {
            // If client is in end state -> do not send him messages
            item = queue_next(item);
            continue;
        }
        if (cmp_clients(client, item) || strcmp(item->data.client.channel_id, channel)) {
            // Skip if message sender or client with another channel
            item = queue_next(item);
            continue;
        }
        
        if (item->data.client.protocol == e_tcp) {
            send_msg(item->data.client.protocol, item->data.client.sockfd, item->data.client.addr, msg, false);
        } else {
            send_msg(item->data.client.protocol, item->data.client.sockfd, item->data.client.addr, msg, false);
            // TODO add msg to queue
        }
        item = queue_next(item);
    }
    
    return 0;
}

void notify_join(queue_item_t * client) {
    msg_t not;
    memset(&not, '\0', sizeof(not));
    not.type = e_msg;
    snprintf((char *)&(not.data.msg.display_name), sizeof(not.data.msg.display_name), "%s", "Server");
    snprintf((char *)&(not.data.msg.content), sizeof(not.data.msg.content), "User %s joined %s.", client->data.client.display_name, client->data.client.channel_id);
    forward_msg_channel(client, not, client->data.client.channel_id);
}

void notify_leave(queue_item_t * client) {
    msg_t not;
    memset(&not, '\0', sizeof(not));
    not.type = e_msg;
    snprintf((char *)&(not.data.msg.display_name), sizeof(not.data.msg.display_name), "%s", "Server");
    snprintf((char *)&(not.data.msg.content), sizeof(not.data.msg.content), "User %s left %s.", client->data.client.display_name, client->data.client.channel_id);
    forward_msg_channel(client, not, client->data.client.channel_id);
}

void send_error(transport_protocol_t protocol, int sockfd, struct sockaddr_in addr, message_content_t err_msg) {
    msg_t msg;
    msg.type = e_err;
    memcpy(&(msg.data.err.display_name), "Server", 6);
    memcpy(&(msg.data.err.content), err_msg, sizeof(*err_msg));
    send_msg(protocol, sockfd, addr, msg, false);
}

void send_msg(transport_protocol_t protocol, int sockfd, struct sockaddr_in addr, msg_t msg, bool is_retransmitted) {
    string_t buf;
    int buf_size = 0;
    message_id_t id = msg_cnt++;
    msg.id = id;
    compose(protocol, msg, &buf, &buf_size);
    server_send(protocol, sockfd, addr, buf, buf_size);
    fprintf(stdout, "SENT %s:%d | ", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
    print_msg_type(msg.type);
    fprintf(stdout, "\n");
    if (protocol == e_udp && msg.type != e_confirm && !is_retransmitted) {
        // Add message to confirm queue
        // If it is not confirm or old retransmitted message
        queue_item_t * item = queue_create_item();
        item->id = id;
        item->data.msg.id = id;
        item->type = e_item_msg;
        item->data.msg = msg;
        item->data.msg.sockfd = sockfd;
        item->data.msg.addr = addr;
        item->data.msg.timeout = argv.udp_timeout;
        item->data.msg.retry_count = 0;
        item->data.msg.is_confirmed = false;
        queue_add(msg_out_buf, item);
    }
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

int close_client(queue_item_t * client) {
    msg_t msg;
    memset(&msg, '\0', sizeof(msg));
    msg.type = e_bye;
    send_msg(client->data.client.protocol, client->data.client.sockfd, client->data.client.addr, msg, false);
    if (client->data.client.protocol == e_tcp) {
        delete_client(client);
    }
    // If udp -> client will be deleted when confirm will arrive
    return 0;
}

int delete_client(queue_item_t * client) { // TODO delete messages from msg_out_buf
    del_poll_fd(&pollfd, client->data.client.sockfd);
    queue_destroy_item(queue_remove(clients, client->id));
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