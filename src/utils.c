/**
 * @file utils.c
 * @author Sniehovskyi Nikita (xsnieh00)
 * @date 14.04.2024
 * @brief Operations upon server data.
 */

#include "utils.h"

int get_free_poll_fd(pollfd_list_t * pollfd, int * index) {
    *index = 0;
    for (int i = WELCOME_SOCK_COUNT - 1; i < pollfd->size; i++) {
        if ((pollfd->pollfd_list)[i].fd == 0) {
            *index = i;
            return 0;
        }
    }
    if (*index == 0 && resize_poll_fd(pollfd)) {
        return errno;
    }
    *index = pollfd->size - POLL_REALLOC_DELTA; // First index of new space
    return 0;
}

int resize_poll_fd(pollfd_list_t * pollfd) {
    int new_size = pollfd->size + POLL_REALLOC_DELTA;
    pollfd->pollfd_list = realloc(pollfd->pollfd_list, new_size * sizeof(struct pollfd));
    if (pollfd->pollfd_list == NULL) {
        return errno = error_out(error_memo_mem_alloc_fail, __LINE__, __FILE__, NULL);
    }
    memset(&(pollfd->pollfd_list[pollfd->size]), '\0', sizeof(struct pollfd) * POLL_REALLOC_DELTA);
    pollfd->protocol = realloc(pollfd->protocol, new_size * sizeof(transport_protocol_t));
    if (pollfd->protocol == NULL) {
        return errno = error_out(error_memo_mem_alloc_fail, __LINE__, __FILE__, NULL);
    }
    memset(&(pollfd->protocol[pollfd->size]), '\0', sizeof(transport_protocol_t) * POLL_REALLOC_DELTA);
    pollfd->size = new_size;
    return 0;
}

queue_item_t * get_client_by_addr(queue_t * clients, struct sockaddr_in client_addr) {
    queue_item_t * item = NULL;
    item = queue_first(clients);
    while (item != NULL) {
        if (is_equal_addr(item->data.client.addr, client_addr)) {
            return item;
        }
        item = queue_next(item);
    }
    return NULL;
}

queue_item_t * get_client_by_fd(queue_t * clients, int fd) {
    queue_item_t * item = NULL;
    item = queue_first(clients);
    while (item != NULL) {
        if (item->data.client.sockfd == fd) {
            return item;
        }
        item = queue_next(item);
    }
    return NULL;
}

bool is_equal_addr(struct sockaddr_in addr1, struct sockaddr_in addr2) {
    if (addr1.sin_addr.s_addr != addr2.sin_addr.s_addr) {
        return false;
    }
    if (addr1.sin_family != addr2.sin_family) {
        return false;
    }
    if (addr1.sin_port != addr2.sin_port) {
        return false;
    }
        
    return true;
}

int add_poll_fd(pollfd_list_t * pollfd, int new_fd, transport_protocol_t prot) {
    int index = 0;
    if (get_free_poll_fd(pollfd, &index)) {
        return errno;
    }
    pollfd->cnt++;
    (pollfd->pollfd_list)[index].fd = new_fd;
    (pollfd->protocol)[index] = prot;
    (pollfd->pollfd_list)[index].events = POLLIN;
    return 0;
}

int del_poll_fd(pollfd_list_t * pollfd, int fd) {
    int i = WELCOME_SOCK_COUNT;
    int i1 = 0;
    int i2 = 0;
    while (i < pollfd->size && (!i1 || !i2)) {
        if (fd == pollfd->pollfd_list[i].fd) {
            i1 = i;
        }
        if (pollfd->pollfd_list[i].fd == 0) {
            i2 = i;
        }
        i++;
    }
    if (i1 == 0) {
        // Not found -> does not exist
        return 0; 
    }
    if (i2 == 0) {
        i2 = pollfd->size - 1;
    } else {
        i2--;
        if (i2 == WELCOME_SOCK_COUNT - 1) {
            i2 = i1;
        }
    }
    close(pollfd->pollfd_list[i].fd); // Close file descriptor
    pollfd->pollfd_list[i1].fd = pollfd->pollfd_list[i2].fd;
    pollfd->protocol[i1] = pollfd->protocol[i2];
    pollfd->pollfd_list[i2].fd = 0;
    pollfd->protocol[i2] = 0;
    pollfd->cnt--;
    return 0;
}

int get_next_timeout(queue_t * msg) {
    int ret = 0;
    queue_item_t * item = queue_first(msg);

    while (item != NULL) {
        if (item->data.msg.is_confirmed == false && item->data.msg.timeout < ret) {
            ret = item->data.msg.timeout;
        }
        item = queue_next(item);
    }
    return ret;
}

void msg_fill_error(msg_t * msg, display_name_t dname, message_content_t cont) {
    msg->type = e_err;
    memcpy(msg->data.err.display_name, dname, strlen(dname));
    memcpy(msg->data.err.content, cont, strlen(cont));
}

void msg_fill_reply(msg_t * msg, bool result, message_id_t ref_id, message_content_t cont) {
    msg->type = e_reply;
    msg->data.reply.ref_id = ref_id;
    msg->data.reply.result = result;
    memcpy(msg->data.err.content, cont, strlen(cont));
}

bool cmp_clients(queue_item_t * client1, queue_item_t * client2) {
    if (client1->data.client.sockfd != client2->data.client.sockfd) {
        return false;
    }
    if (strcmp(client1->data.client.username, client2->data.client.username)) {
        return false;
    }

    return true;
}

void print_msg_type(msg_type_t type) {
    switch (type) {
        case e_confirm: fprintf(stdout, "CONFIRM"); break;
        case e_reply: fprintf(stdout, "REPLY"); break;
        case e_auth: fprintf(stdout, "AUTH"); break;
        case e_join: fprintf(stdout, "JOIN"); break;
        case e_msg: fprintf(stdout, "MSG"); break;
        case e_err: fprintf(stdout, "ERR"); break;
        case e_bye: fprintf(stdout, "BYE"); break;
    }
}