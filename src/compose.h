/**
 * @file compose.h
 * @author Sniehovskyi Nikita (xsnieh00)
 * @date 18.04.2024
 * @brief Composer of network encoded messages.
 */

#ifndef __COMPOSE_H__

#define __COMPOSE_H__

#include <stdlib.h>
#include <stdio.h>

#include "types.h"
#include "error.h"

/**
 * @brief Described in 'main.h' 
 */
extern int errno;

/**
 * @brief Routing function for message composer. Composes internal message type into protocol defined format
 * 
 * @param protocol Protocol type (how to decode message)
 * @param msg_in Received internal message
 * @param msg_out Composed message
 * @param msg_out_size Composed message length (only for UDP)
 * @return int Result code
 */
int compose(transport_protocol_t protocol, msg_t msg_in, string_t * msg_out, int * msg_out_size);

/**
 * @brief Composes internal message type into TCP protocol format
 * 
 * @param msg_in Received internal message
 * @param msg_out Composed message
 * @return int Result code
 */
int compose_tcp(msg_t msg_in, string_t * msg_out);

/**
 * @brief Composes internal message type into UDP protocol format
 * 
 * @param msg_in Received internal message
 * @param msg_out Composed message
 * @param msg_out_size Composed message length
 * @return int Result code
 */
int compose_udp(msg_t msg_in, string_t * msg_out, int * msg_out_size);

#endif