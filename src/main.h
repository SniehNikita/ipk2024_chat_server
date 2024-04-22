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
#include <unistd.h> 

#include "types.h"
#include "queue.h"
#include "error.h"
#include "argv.h"
#include "server.h"
#include "utils.h"
#include "parse.h"
#include "compose.h"

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
 * @brief Does all pre-execution steps on message (like confirmation, state checks) and calls execution function.
 * 
 * @param client Client which sent message
 * @param msg_in Message sent by client
 * @return int Result code
 */
int process_msg(queue_item_t * client, msg_t msg_in);

/**
 * @brief Initiates processing of message received on given file descriptor
 * 
 * @param fd File descriptor with mesage
 * @return int Result code
 */
int process_msg_sock(int fd);

/**
 * @brief Handles UDP timeout. Retransmitts messages than need to be retransmitted
 * 
 * @param interval Time interval passed from previous call
 * @return int Result code
 */
int udp_timeout(int interval);

/**
 * @brief Executes message purpose
 * 
 * @param client Client which sent message
 * @param msg_in Message received from client
 * @param msg_out Answer to the client
 * @return true Answer needs to be send
 * @return false Answer doesn't need to be send
 */
bool execute_msg(queue_item_t * client, msg_t msg_in, msg_t * msg_out);

/**
 * @brief Forwards message to all clients except sender which are currently listen on this channel
 * 
 * @param client Sender of message
 * @param msg Message to forward
 * @param channel Channel where message should be forwarded
 * @return int 
 */
int forward_msg_channel(queue_item_t * client, msg_t msg, channel_id_t channel);

/**
 * @brief Checks if message can be executed within current client state. If no -> sends error to client
 * 
 * @param client Sender of message
 * @param msg Message received
 * @return true Message can be executed
 * @return false Message cannot be executed
 */
bool check_state_transition(queue_item_t * client, msg_t msg);

/**
 * @brief Notifies all clients from client channel that he joined it
 * 
 * @param client Client which joined the channel
 */
void notify_join(queue_item_t * client);

/**
 * @brief Notifies all clients from client channel that he left it
 * 
 * @param client Client which left the channel
 */
void notify_leave(queue_item_t * client);

/**
 * @brief Generates and sends error message.
 * 
 * @param protocol Protocol to use in communication
 * @param sockfd Socket where to send message
 * @param addr Address where to send message
 * @param err_msg Error message content
 */
void send_error(transport_protocol_t protocol, int sockfd, struct sockaddr_in addr, message_content_t err_msg);

/**
 * @brief Isolation point for message sending. Logs sent messages. Fills all common message data like ids, timeout, etc.
 * 
 * @param protocol Protocol to use in communication
 * @param sockfd Socket where to send message
 * @param addr Address where to send message
 * @param msg Message to send
 * @param is_retransmitted Is message retransmitted or it is sent for the first time (should it be saved in msg_out_buf queue) 
 * @return int Result code
 */
int send_msg(transport_protocol_t protocol, int sockfd, struct sockaddr_in addr, msg_t msg, bool is_retransmitted);

/**
 * @brief Isolation point for message reading. Logs read messages.
 * 
 * @param protocol Protocol to use in communication
 * @param sockfd Socket where to read message
 * @param addr Address where to read message
 * @param msg Pointer where to save message
 * @return int 
 */
int read_msg(transport_protocol_t protocol, int sockfd, struct sockaddr_in * addr, msg_t * msg);

/**
 * @brief Prepares client to be deleted. Sends message about connection break.
 * 
 * @param client Client to be marked for deletion
 * @return int Result code
 */
int close_client(queue_item_t * client);

/**
 * @brief Deletes client data
 * 
 * @param client Client to be deleted
 * @return int Resutl code
 */
int delete_client(queue_item_t * client);

/**
 * @brief Hadler for sigint signal
 * 
 * @param signal Signal received from the system
 */
void  sigint_handler(int signal);

/**
 * @brief Hadler for sigpipe signal
 * 
 * @param signal Signal received from the system
 */
void  sigpipe_handler(int signal);

/**
 * @brief Exits as standard exit() function but with freeing of memory
 * 
 * @param exit_code Exit code
 */
void stop(int exit_code);

#endif