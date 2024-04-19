/**
 * @file compose.c
 * @author Sniehovskyi Nikita (xsnieh00)
 * @date 19.04.2024
 * @brief Composer of network messages.
 */

#include "compose.h"

int compose(transport_protocol_t protocol, msg_t msg_in, string_t * msg_out, int * msg_out_size) {
    if (protocol == e_tcp) {
        return compose_tcp(msg_in, msg_out);
    } else {
        return compose_udp(msg_in, msg_out, msg_out_size);
    }
}

int compose_tcp(msg_t msg_in, string_t * msg_out) {
    switch(msg_in.type) {
        case e_confirm:
            return errno = error_out(error_comp_tcp_no_confirm, __LINE__, __FILE__, NULL);
            break;
        case e_reply:
            sprintf(*msg_out, "REPLY %s IS %s\r\n", msg_in.data.reply.result ? "OK" : "NOK", msg_in.data.reply.content);
            break;
        case e_auth:
            sprintf(*msg_out, "AUTH %s AS %s USING %s\r\n", msg_in.data.auth.username, msg_in.data.auth.display_name, msg_in.data.auth.secret);
            break;
        case e_join:
            sprintf(*msg_out, "JOIN %s AS %s\r\n", msg_in.data.join.channel_id, msg_in.data.join.display_name);
            break;
        case e_msg:
            sprintf(*msg_out, "MSG FROM %s IS %s\r\n", msg_in.data.msg.display_name, msg_in.data.msg.content);
            break;
        case e_err:
            sprintf(*msg_out, "ERR FROM %s IS %s\r\n", msg_in.data.msg.display_name, msg_in.data.msg.content);
            break;
        case e_bye:
            sprintf(*msg_out, "BYE\r\n");
            break;
    }
    return 0;
}

int compose_udp(msg_t msg_in, string_t * msg_out, int * msg_out_size) {
    switch(msg_in.type) {
        case e_confirm:
            sprintf(*msg_out, "%c%c%c", e_confirm, msg_in.data.confirm.ref_id >> 8, msg_in.data.confirm.ref_id & 0x00FF);
            *msg_out_size = 3;
            break;
        case e_reply:
            sprintf(*msg_out, "%c%c%c%c%c%c%s%c", e_reply, msg_in.id >> 8, msg_in.id & 0x00FF, msg_in.data.reply.result, msg_in.data.reply.ref_id & 0xFF00, msg_in.data.reply.ref_id & 0x00FF, msg_in.data.reply.content, '\0');
            *msg_out_size = 7 + strlen(msg_in.data.reply.content);
            break;
        case e_auth:
            sprintf(*msg_out, "%c%c%c%s%c%s%c%s%c", e_auth, msg_in.id >> 8, msg_in.id & 0x00FF, msg_in.data.auth.username, '\0', msg_in.data.auth.display_name, '\0', msg_in.data.auth.secret, '\0');
            *msg_out_size = 6 + strlen(msg_in.data.auth.username) + strlen(msg_in.data.auth.display_name) + strlen(msg_in.data.auth.secret);
            break;
        case e_join:
            sprintf(*msg_out, "%c%c%c%s%c%s%c", e_join, msg_in.id >> 8, msg_in.id & 0x00FF, msg_in.data.join.channel_id, '\0', msg_in.data.join.display_name, '\0');
            *msg_out_size = 5 + strlen(msg_in.data.join.channel_id) + strlen(msg_in.data.join.display_name);
            break;
        case e_msg:
            sprintf(*msg_out, "%c%c%c%s%c%s%c", e_msg, msg_in.id >> 8, msg_in.id & 0x00FF, msg_in.data.msg.display_name, '\0', msg_in.data.msg.content, '\0');
            *msg_out_size = 5 + strlen(msg_in.data.msg.display_name) + strlen(msg_in.data.msg.content);
            break;
        case e_err:
            sprintf(*msg_out, "%c%c%c%s%c%s%c", e_err, msg_in.id >> 8, msg_in.id & 0x00FF, msg_in.data.msg.display_name, '\0', msg_in.data.msg.content, '\0');
            *msg_out_size = 5 + strlen(msg_in.data.msg.display_name) + strlen(msg_in.data.msg.content);
            break;
        case e_bye:
            sprintf(*msg_out, "%c%c%c", e_bye, msg_in.id >> 8, msg_in.id & 0x00FF);
            *msg_out_size = 3;
            break;
    }
    return 0;
}