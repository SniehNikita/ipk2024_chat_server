/**
 * @file queue.h
 * @author Sniehovskyi Nikita (xsnieh00)
 * @date 24.03.2024
 * @brief Header file for queue implementation
 *
 * Queue for messages managing
 */


#ifndef __QUEUE_H__

#define __QUEUE_H__

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "error.h"
#include "types.h"

/**
 * @brief Described in 'main.h' 
 */
extern int errno;

typedef enum {
    e_item_msg,
    e_item_client
} queue_item_type_t;

typedef union queue_item_content_t {
    msg_t msg;
    client_t client;
} queue_item_content_t;

typedef struct queue_item_t {
    message_id_t id; // Id of queue item
    queue_item_type_t type; // Type of item in queue
    msg_t msg;
    struct queue_item_t * next;
} queue_item_t;

typedef struct queue_t {
    queue_item_t * first;
} queue_t;

queue_t * queue_create();

queue_item_t * queue_create_item();

int queue_length(queue_t * queue);

void queue_add(queue_t * queue, queue_item_t * item);

queue_item_t * queue_first(queue_t * queue);

queue_item_t * queue_next(queue_item_t * item);

queue_item_t * queue_get(queue_t * queue, message_id_t id);

queue_item_t * queue_remove(queue_t * queue, message_id_t id);

queue_item_t * queue_last(queue_t * queue);

void queue_destroy(queue_t * queue);

void queue_destroy_item(queue_item_t * item);

#endif