/**
 * @file queue.c
 * @author Sniehovskyi Nikita (xsnieh00)
 * @date 24.03.2024
 * @brief Message queue implementation 
 */

#include "queue.h"

queue_t * queue_create() {
    queue_t * queue = malloc(sizeof(queue_t));
    if (queue == NULL) {
        errno = error_out(error_memo_mem_alloc_fail, __LINE__, __FILE__, NULL);
        return NULL;
    }
    memset(&(*queue), '\0', sizeof(queue_t));
    queue->first = NULL;
    return queue;
}

queue_item_t * queue_create_item() {
    queue_item_t * item = malloc(sizeof(queue_item_t));
    if (item == NULL) {
        errno = error_out(error_memo_mem_alloc_fail, __LINE__, __FILE__, NULL);
        return NULL;
    }
    memset(&(*item), '\0', sizeof(queue_item_t));
    item->next = NULL;
    return item;
}

int queue_length(queue_t * queue) {
    int cnt = 0;
    queue_item_t * item = queue_first(queue);
    while (item != NULL) {
        cnt++;
        item = item->next;
    }
    return cnt;
}

void queue_add(queue_t * queue, queue_item_t * item) {
    if (queue_last(queue) == NULL) {
        queue->first = item;
    } else {
        queue_last(queue)->next = item;
    }    
}

queue_item_t * queue_first(queue_t * queue) {
    return queue->first;
}

queue_item_t * queue_next(queue_item_t * item) {
    return item->next;
}

queue_item_t * queue_get(queue_t * queue, id_t id) {
    queue_item_t * item = queue->first;
    while (item != NULL && item->id != id) {
        item = item->next;
    }
    return item;
}

queue_item_t * queue_remove(queue_t * queue, id_t id) {
    queue_item_t * last_item = queue->first;
    queue_item_t * item;

    if (last_item == NULL) {
        return last_item;
    }
    if (last_item->id == id) {
        queue->first = queue->first->next;
        return last_item;
    }
    item = last_item->next;
    while (item != NULL) {
        if (item->id == id) {
            last_item->next = item->next;
            break;
        }
        last_item = item;
        item = item->next;
    }
    return item;
}

queue_item_t * queue_last(queue_t * queue) {
    queue_item_t * item = queue->first;
    
    if (item == NULL) {
        return item;
    }
    while (item->next != NULL) {
        item = item->next;
    }
    return item;
}

void queue_destroy(queue_t * queue) {
    if (queue != NULL) { 
        while (queue->first != NULL) {
            queue_destroy_item(queue_remove(queue, queue->first->id));
        }
        free(queue);
        queue = NULL;
    }
}

void queue_destroy_item(queue_item_t * item) {
    if (item != NULL) {
        free(item);
        item = NULL;
    }
}