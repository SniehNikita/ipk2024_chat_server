/**
 * @file parse.h
 * @author Sniehovskyi Nikita (xsnieh00)
 * @date 18.04.2024
 * @brief Parser of network encoded messages.
 */

#ifndef __PARSE_H__

#define __PARSE_H__

#include <stdlib.h>
#include <stdio.h>
#include <regex.h>

#include "types.h"
#include "error.h"

/**
 * @brief Described in 'main.h' 
 */
extern int errno;

/**
 * @brief Routing function for message parser. Takes 1 encoded message and decodes it acording to protocol encoding specification
 * 
 * @param protocol Protocol type (how to decode message)
 * @param msg_in Received encoded message
 * @param msg_in_size Received message length
 * @param msg_out Parsed message
 * @return int Result code
 */
int parse(transport_protocol_t protocol, string_t msg_in, int msg_in_size, msg_t * msg_out);

int parse_tcp(string_t msg_in, msg_t * msg_out);

/**
 * @brief Parses UDP message
 * 
 * @param msg_in Received message
 * @param msg_in_size Received message length
 * @param msg_out Parsed message
 * @return int Result code
 */
int parse_udp(string_t msg_in, int msg_in_size, msg_t * msg_out);

/**
 * @brief Splits string from s at c into ptr array (maximum max substrings). End of string is considered on len character
 * 
 * @param str String to split
 * @param c Char, at which string should be splitted
 * @param s Starting index
 * @param ptr Array where to store substrings
 * @param max Maximum number of substrings
 * @param len Index of last char of string
 * @return int Index of last substring in ptr array
 */
int split_l(string_t str, char c, int s, string_t ** ptr, int max, int len);

#endif