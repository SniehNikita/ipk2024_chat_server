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

#endif