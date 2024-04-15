/**
 * @file server.c
 * @author Sniehovskyi Nikita (xsnieh00)
 * @date 11.04.2024
 * @brief Handler of network part of the server. 
 */

#include "server.h"

int sockfd_tcp_welcome;
int sockfd_udp_welcome;
struct sockaddr_in addr;

int server_open() {
    int enable = 1;
    
    sockfd_tcp_welcome = socket(AF_INET, SOCK_STREAM, 0);
    sockfd_udp_welcome = socket(AF_INET, SOCK_DGRAM, 0);

    if (sockfd_tcp_welcome <= 0 || sockfd_udp_welcome <= 0) {
        return errno = error_out(error_serv_sock_init_fail, __LINE__, __FILE__, NULL);
    }
    
    setsockopt(sockfd_tcp_welcome, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable));
    if (fcntl(sockfd_tcp_welcome, F_SETFL, fcntl(sockfd_tcp_welcome, F_GETFL, 0) | O_NONBLOCK)) {
        return errno = error_out(error_serv_sock_init_fail, __LINE__, __FILE__, NULL);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(argv.ipv4); // TODO This or inaddr_any ?
    addr.sin_port = htons(argv.port);

    if (bind(sockfd_tcp_welcome, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        return errno = error_out(error_serv_addr_bind_fail, __LINE__, __FILE__, NULL);
    }
    if (bind(sockfd_udp_welcome, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        return errno = error_out(error_serv_addr_bind_fail, __LINE__, __FILE__, NULL);
    }

    if (listen(sockfd_tcp_welcome, MAX_BACKLOG_CNT) == -1) {
        return errno = error_out(error_serv_tcp_listn_fail, __LINE__, __FILE__, NULL);
    }

    return 0;
}

int server_accept(transport_protocol_t protocol, int * client_sockfd, struct sockaddr_in * client_addr) {
    uint32_t addr_in_size = sizeof(*client_addr);
    switch (protocol) {
        case e_tcp:
            if ((*client_sockfd = accept(sockfd_tcp_welcome, (struct sockaddr *)client_addr, &addr_in_size)) == -1) {
                return errno = error_out(error_serv_tcp_accpt_fail, __LINE__, __FILE__, NULL);        
        	}
            if (fcntl(*client_sockfd, F_SETFL, fcntl(*client_sockfd, F_GETFL, 0) | O_NONBLOCK)) {
                return errno = error_out(error_serv_sock_init_fail, __LINE__, __FILE__, NULL);
            }
            break;
        case e_udp:

            break;
    }
    return 0;
}

int server_read(transport_protocol_t protocol, int client_sockfd, string_t * buf, int * size) {
    switch(protocol) {
        case e_tcp:
            *size = recv(client_sockfd, *buf, sizeof(*buf), 0);
            break;
        case e_udp:

            break;
    }
    return 0;
}

int server_close_client(int fd) {
    close(fd);
    return 0;
}

int server_close() {
    close(sockfd_tcp_welcome);
    close(sockfd_udp_welcome);
    return 0;
}

int get_tcp_socket() {
    return sockfd_tcp_welcome;
}

int get_udp_socket() {
    return sockfd_udp_welcome;
}