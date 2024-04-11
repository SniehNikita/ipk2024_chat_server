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

#include "types.h"
#include "error.h"

/**
 * @brief Described in 'main.h' 
 */
extern int errno;

/**
 * @brief Described in 'main.h' 
 */
extern argv_t argv;

#endif