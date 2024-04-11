/**
 * @file types.h
 * @author Sniehovskyi Nikita (xsnieh00)
 * @date 11.04.2024
 * @brief Definition of all custom types used in server and type checks.
 */

#ifndef __TYPES_H__

#define __TYPES_H__

#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <regex.h>
#include <sys/socket.h>
#include <netinet/in.h>

/**
 * @brief Maximum length of IPV4 address 
 */
#define IPV4_MAX_LENGTH 16

/**
 * @brief Maximum length of username 
 */
#define USERNAME_MAX_LENGTH 20

/**
 * @brief Maximum length of channel id 
 */
#define CHANNELID_MAX_LENGTH 20

/**
 * @brief Maximum length of secret
 */
#define SECRET_MAX_LENGTH 128

/**
 * @brief Maximum length of display name
 */
#define DISPLAYNAME_MAX_LENGTH 20

/**
 * @brief Maximum length of message content
 */
#define MESSAGECONTENT_MAX_LENGTH 1400

/**
 * @brief Maximum length of common use string 
 */
#define STRING_MAX_LENGTH 2048

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

/**
 * @brief Message ID uint type 
 */
typedef uint16_t message_id_t;

/**
 * @brief Username string type 
 */
typedef char username_t[USERNAME_MAX_LENGTH];

/**
 * @brief Channel ID string type
 * 
 */
typedef char channel_id_t[CHANNELID_MAX_LENGTH];

/**
 * @brief Secret string type
 * 
 */
typedef char secret_t[SECRET_MAX_LENGTH];

/**
 * @brief Displayname string type
 * 
 */
typedef char display_name_t[DISPLAYNAME_MAX_LENGTH];

/**
 * @brief Message content string type 
 */
typedef char message_content_t[MESSAGECONTENT_MAX_LENGTH];

/**
 * @brief Common use string 
 */
typedef char string_t[STRING_MAX_LENGTH];

/**
 * @brief Message type constants 
 */
typedef enum {
    e_confirm = 0x00,
    e_reply = 0x01,
    e_auth = 0x02,
    e_join = 0x03,
    e_msg = 0x04,
    e_err = 0xFE,
    e_bye = 0xFF
} msg_type_t;

/**
 * @brief Content of confirm message 
 */
typedef struct msg_confirm_t {
    message_id_t ref_id;
} msg_confirm_t;

/**
 * @brief Content of reply message 
 */
typedef struct msg_reply_t {
    bool result;
    message_id_t ref_id;
    message_content_t content;
} msg_reply_t;

/**
 * @brief Content of auth message 
 */
typedef struct msg_auth_t {
    username_t username;
    display_name_t display_name;
    secret_t secret;
} msg_auth_t;

/**
 * @brief Content of join message 
 */
typedef struct msg_join_t {
    channel_id_t channel_id;
    display_name_t display_name;
} msg_join_t;

/**
 * @brief Content of msg message 
 */
typedef struct msg_msg_t {
    display_name_t display_name;
    message_content_t content;
} msg_msg_t;

/**
 * @brief Content of err message 
 */
typedef struct msg_err_t {
    display_name_t display_name;
    message_content_t content;
} msg_err_t;

/**
 * @brief Union of all possible message contents 
 */
typedef union msg_data_t {
    msg_confirm_t confirm;
    msg_reply_t reply;
    msg_auth_t auth;
    msg_join_t join;
    msg_msg_t msg;
    msg_err_t err;
} msg_data_t;

/**
 * @brief Unified communication message
 */
typedef struct t_msg {
    msg_type_t type;
    message_id_t id;
    msg_data_t data;
} t_msg;

/**
 * @brief Record about client 
 */
typedef struct client_t {
    int socket_fd; // file descriptor of assigned socket
    struct sockaddr_in addr; // Address of client
    display_name_t display_name;
    channel_id_t channel_id;
} client_t;

/**
 * @brief Checks if given string is message id
 * 
 * @param str String to check
 * @return true String is message id
 * @return false String is not message id
 */
bool is_message_id(string_t str);

/**
 * @brief Checks if given string is username
 * 
 * @param str String to check
 * @return true String is username
 * @return false String is not username
 */
bool is_username(string_t str);

/**
 * @brief Checks if given string is channel id
 * 
 * @param str String to check
 * @return true String is channel id
 * @return false String is not channel id
 */
bool is_channel_id(string_t str);

/**
 * @brief Checks if given string is secret
 * 
 * @param str String to check
 * @return true String is secret
 * @return false String is not secret
 */
bool is_secret(string_t str);

/**
 * @brief Checks if given string is display name
 * 
 * @param str String to check
 * @return true String is display name
 * @return false String is not display name
 */
bool is_display_name(string_t str);

/**
 * @brief Checks if given string is message content
 * 
 * @param str String to check
 * @return true String is message content
 * @return false String is not message content
 */
bool is_message_content(string_t str);

#endif