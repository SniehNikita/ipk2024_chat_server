/**
 * @file main.h
 * @author Sniehovskyi Nikita (xsnieh00)
 * @date 11.04.2024
 * @brief Root program logic.
 */

#ifndef __MAIN_H__

#define __MAIN_H__

#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <poll.h>

#include "types.h"
#include "queue.h"
#include "error.h"
#include "argv.h"
#include "server.h"
#include "utils.h"

/**
 * @brief Global error flag, should be set in case error occured with relevant error code 
 */
int errno = 0;

/**
 * @brief Global constant. Arguments of the server, changed once at the start of the program
 */
argv_t argv;

/**
 * @brief Constant help message string 
 */
char help_msg[] = "Usage:\n./ipk24char-server [-l <ipv4>] [-p <port>] [-d <timeout>] [-r <retransmissions>] [-h]\n\t-l\t0.0.0.0\t-\tIP v4 address\t:\tServer listening IP address for welcome sockets\n\t-p\t4567\t-\tuint16\t\t:\tServer listening port for welcome sockets\n\t-d\t250\t-\tuint16\t\t:\tUDP confirmation timeout\n\t-r\t3\t-\tuint8\t\t:\tMaximum number of UDP retransmissions\n\t-h\t\t-\t\t\t:\tPrints program help output and exits";

/**
 * @brief Handler of polling on tcp sockfd. Registers new client
 * 
 * @return int Result code
 */
int tcp_polling();

/**
 * @brief Handler of polling on udp sockfd. Registers new client
 * 
 * @return int Result code
 */
int udp_polling();

/**
 * @brief Processing received message from clients
 * 
 * @param fd File descriptor with message
 * @return int Result code
 */
int process_msg(int fd);

/**
 * @brief Hadler for sigint signal
 * 
 * @param signal Signal received from the system
 */
void  sigintHandler(int signal);

/**
 * @brief Exits as standard exit() function but with freeing of memory
 * 
 * @param exit_code Exit code
 */
void stop(int exit_code);

#endif