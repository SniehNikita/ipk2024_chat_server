/**
 * @file utils.h
 * @author Sniehovskyi Nikita (xsnieh00)
 * @date 14.04.2024
 * @brief Operations upon server data.
 */

#ifndef __UTILS_H__

#define __UTILS_H__

#include <stdlib.h>
#include <stdio.h>
#include <poll.h>
#include <unistd.h> 

#include "types.h"
#include "queue.h"
#include "error.h"

/**
 * @brief Described in 'main.h' 
 */
extern int errno;

/**
 * @brief Returns next available fd in pollfd list
 * 
 * @param pollfd Pollfd structure
 * @param index Index of free fd in pollfd list
 * @return int Result code
 */
int get_free_poll_fd(pollfd_list_t * pollfd, int * index);

/**
 * @brief Resizes poll fd list if exceeded number of fds
 * 
 * @param pollfd Pollfd structure
 * @return int Result code
 */
int resize_poll_fd(pollfd_list_t * pollfd);

/**
 * @brief Searches for client with given address in clients list.
 * 
 * @param clients List of clients
 * @param client_addr Address of searched client
 * @return queue_item_t* NULL if not found. Pointer to item in queue if found
 */
queue_item_t * get_client_by_addr(queue_t * clients, struct sockaddr_in client_addr);

/**
 * @brief Searches for client with given fd in clients list.
 * 
 * @param clients List of clients
 * @param fd File descriptor of searched client
 * @return queue_item_t* NULL if not found. Pointer to item in queue if found
 */
queue_item_t * get_client_by_fd(queue_t * clients, int fd);

/**
 * @brief Comparion of addresses
 * 
 * @param addr1 Address 1
 * @param addr2 Address 2
 * @return true Addresses are equal
 * @return false Addresses are not equal
 */
bool is_equal_addr(struct sockaddr_in addr1, struct sockaddr_in addr2);

/**
 * @brief Adds fd to pollfd list
 * 
 * @param pollfd Pollfd list
 * @param new_fd Fd to be added
 * @param prot Transport protocol
 * @return int Result code
 */
int add_poll_fd(pollfd_list_t * pollfd, int new_fd, transport_protocol_t prot);

/**
 * @brief Delete fd from pollfd list
 * 
 * @param pollfd Pollfd list
 * @param fd Fd to be deleted
 * @return int Result code
 */
int del_poll_fd(pollfd_list_t * pollfd, int fd);

/**
 * @brief Selects lowest time to timeout from message queue and returns it
 * 
 * @param msg Message queue
 * @return int Time in ms until next message confirmation will timeout
 */
int get_next_timeout(queue_t * msg);

/**
 * @brief Fills error message data
 * 
 * @param msg Pointer to message
 * @param dname Display name
 * @param cont Content
 */
void msg_fill_error(msg_t * msg, display_name_t dname, message_content_t cont);

/**
 * @brief Fills reply message data
 * 
 * @param msg Pointer to message
 * @param result Result
 * @param ref_id Reffered id
 * @param cont Content
 */
void msg_fill_reply(msg_t * msg, bool result, message_id_t ref_id, message_content_t cont);

/**
 * @brief Compares 2 clients
 * 
 * @param client1 Client 1 to compare
 * @param client2 Client 2 to compare
 * @return true Clients are the same
 * @return false Clients are different
 */
bool cmp_clients(queue_item_t * client1, queue_item_t * client2);

bool is_msg_confirmed(queue_item_t * client, message_id_t id);

/**
 * @brief Prints user-friendly message type to stdout
 * 
 * @param type Encoded message type
 */
void print_msg_type(msg_type_t type);

#endif