/**
 * @file types.h
 * @author Sniehovskyi Nikita (xsnieh00)
 * @date 11.04.2024
 * @brief Definition of all custom types used in server.
 */

#ifndef __TYPES_H__

#define __TYPES_H__

#include <stdbool.h>
#include <string.h>
#include <stdint.h>

/**
 * @brief Maximum length of IPV4 address 
 */
#define IPV4_MAX_LENGTH 16

/**
 * @brief IPV4 address type (string representation) 
 */
typedef char ipv4_t[IPV4_MAX_LENGTH];

/**
 * @brief Server arguments type 
 */
typedef struct argv_t {
    ipv4_t ipv4;
    uint16_t port;
    uint16_t udp_timeout;
    uint8_t udp_retransmissions;
    bool is_help;
} argv_t;

#endif