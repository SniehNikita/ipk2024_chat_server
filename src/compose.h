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

int compose(transport_protocol_t protocol, msg_t msg_in, string_t * msg_out, int * msg_out_size);

int compose_tcp(msg_t msg_in, string_t * msg_out);

int compose_udp(msg_t msg_in, string_t * msg_out, int * msg_out_size);

#endif