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

#include "types.h"
#include "error.h"
#include "argv.h"
#include "server.h"

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

#endif