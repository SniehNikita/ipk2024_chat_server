/**
 * @file server.h
 * @author Sniehovskyi Nikita (xsnieh00)
 * @date 11.04.2024
 * @brief Handler of network part of the server. 
 */

#ifndef __SERVER_H__

#define __SERVER_H__

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <unistd.h> 
#include <netdb.h>
#include <fcntl.h>

#include "types.h"
#include "queue.h"
#include "error.h"

/**
 * @brief Described in 'main.h' 
 */
extern int errno;

/**
 * @brief Described in 'main.h' 
 */
extern argv_t argv;

/**
 * @brief Prepare sockets and bind addres
 * 
 * @return int Result code
 */
int server_open();

/**
 * @brief Accepts new connection on specified protocol
 * 
 * @param protocol Protocol where new connection is waiting
 * @param client_sockfd New connection socked fd 
 * @param client_addr New connection address
 * @return int Result code
 */
int server_accept(transport_protocol_t protocol, int * client_sockfd, struct sockaddr_in * client_addr);

/**
 * @brief Read message on socket
 * 
 * @param client Client data to receive from
 * @param buf Buffered message
 * @param size Size of received message
 * @return int Result code
 */
int server_read(queue_item_t * client, string_t * buf);

int server_read_sock(transport_protocol_t protocol, int sockfd, struct sockaddr_in * addr, string_t * buf);

int server_send(transport_protocol_t protocol, int sockfd, struct sockaddr_in addr, string_t buf, int buf_size);

/**
 * @brief Close communication of fd
 * 
 * @param fd Fd
 * @return int Result code
 */
int server_close_client(int fd);

/**
 * @brief Closes sockets
 * 
 * @return int Result code
 */
int server_close();

/**
 * @brief Get the tcp welcome socket
 * 
 * @return int Socket fd
 */
int get_tcp_socket();

/**
 * @brief Get the udp welcome socket
 * 
 * @return int Socket fd
 */
int get_udp_socket();

#endif