/**
 * @file parse.c
 * @author Sniehovskyi Nikita (xsnieh00)
 * @date 18.04.2024
 * @brief Parser of network encoded messages.
 */

#include "parse.h"

int parse(transport_protocol_t protocol, string_t msg_in, int msg_in_size, msg_t * msg_out) {
    if (protocol == e_tcp) {
        return parse_tcp(msg_in, msg_out);
    } else {
        return parse_udp(msg_in, msg_in_size, msg_out);
    }
}

int parse_tcp(string_t msg_in, msg_t * msg_out) {
    regex_t regex;
    regmatch_t pmatch[4];

    regcomp(&regex, "^JOIN \\([[:alnum:]-]\\{1,21\\}\\) AS \\([[:graph:]]\\{1,21\\}\\)\r\n$", 0);
    if (!regexec(&regex, msg_in, 3, pmatch, 0)) {
        msg_out->type = e_join;
        memcpy(msg_out->data.join.channel_id, msg_in + pmatch[1].rm_so, pmatch[1].rm_eo - pmatch[1].rm_so);
        memcpy(msg_out->data.join.display_name, msg_in + pmatch[2].rm_so, pmatch[2].rm_eo - pmatch[2].rm_so);
        return 0;
    }
    regcomp(&regex, "^AUTH \\([[:alnum:]-]\\{1,21\\}\\) AS \\([[:graph:]]\\{1,21\\}\\) USING \\([[:alnum:]-]\\{1,129\\}\\)\r\n$", 0);
    if (!regexec(&regex, msg_in, 4, pmatch, 0)) {
        msg_out->type = e_auth;
        memcpy(msg_out->data.auth.username, msg_in + pmatch[1].rm_so, pmatch[1].rm_eo - pmatch[1].rm_so);
        memcpy(msg_out->data.auth.display_name, msg_in + pmatch[2].rm_so, pmatch[2].rm_eo - pmatch[2].rm_so);
        memcpy(msg_out->data.auth.secret, msg_in + pmatch[3].rm_so, pmatch[3].rm_eo - pmatch[3].rm_so);
        return 0;
    }
    regcomp(&regex, "^MSG FROM \\([[:graph:]]\\{1,21\\}\\) IS \\([[:print:]]\\{1,1401\\}\\)\r\n$", 0);
    if (!regexec(&regex, msg_in, 3, pmatch, 0)) {
        msg_out->type = e_msg;
        memcpy(msg_out->data.msg.display_name, msg_in + pmatch[1].rm_so, pmatch[1].rm_eo - pmatch[1].rm_so);
        memcpy(msg_out->data.msg.content, msg_in + pmatch[2].rm_so, pmatch[2].rm_eo - pmatch[2].rm_so);
        return 0;
    }
    regcomp(&regex, "^ERR FROM \\([[:graph:]]\\{1,21\\}\\) IS \\([[:print:]]\\{1,1401\\}\\)\r\n$", 0);
    if (!regexec(&regex, msg_in, 3, pmatch, 0)) {
        msg_out->type = e_err;
        memcpy(msg_out->data.err.display_name, msg_in + pmatch[1].rm_so, pmatch[1].rm_eo - pmatch[1].rm_so);
        memcpy(msg_out->data.err.content, msg_in + pmatch[2].rm_so, pmatch[2].rm_eo - pmatch[2].rm_so);
        return 0;
    }
    regcomp(&regex, "^REPLY OK IS \\([[:print:]]\\{1,1401\\}\\)\r\n$", 0);
    if (!regexec(&regex, msg_in, 2, pmatch, 0)) {
        msg_out->type = e_reply;
        msg_out->data.reply.result = true;
        memcpy(msg_out->data.reply.content, msg_in + pmatch[1].rm_so, pmatch[1].rm_eo - pmatch[1].rm_so);
        return 0;
    }
    regcomp(&regex, "^REPLY NOK IS \\([[:print:]]\\{1,1401\\}\\)\r\n$", 0);
    if (!regexec(&regex, msg_in, 2, pmatch, 0)) {
        msg_out->type = e_reply;
        msg_out->data.reply.result = false;
        memcpy(msg_out->data.reply.content, msg_in + pmatch[1].rm_so, pmatch[1].rm_eo - pmatch[1].rm_so);
        return 0;
    }
    regcomp(&regex, "^BYE\r\n$", 0);
    if (!regexec(&regex, msg_in, 0, pmatch, 0)) {
        msg_out->type = e_bye;
        return 0;
    }
    
    return errno = error_out(error_pars_tcp_parse_fail, __LINE__, __FILE__, NULL);
}

int parse_udp(string_t msg_in, int msg_in_size, msg_t * msg_out) {
    string_t * match;
    int m;

    match = malloc(sizeof(string_t) * 3);
    memset(*match, '\0', sizeof(match));
    switch((unsigned char) msg_in[0]) {
        case e_confirm:
            msg_out->type = e_confirm;
            msg_out->data.confirm.ref_id = msg_in[1] * 0xFF + msg_in[2];
            if (msg_in_size == 3) {
                free(match);
                return 0;
            }
            break;
        case e_reply:
            msg_out->type = e_reply;
            msg_out->id = msg_in[1] * 0xFF + msg_in[2];
            msg_out->data.reply.result = msg_in[3];
            msg_out->data.reply.ref_id = msg_in[4] * 0xFF + msg_in[5];
            m = split_l(msg_in, '\0', 6, &match, 1, msg_in_size);
            if (msg_in_size >= 8 && m == 0 && is_message_content(match[0])) {
                memcpy(msg_out->data.reply.content, match[0], strlen(match[0]));
                free(match);
                return 0;
            }
            break;
        case e_auth:
            msg_out->type = e_auth;
            msg_out->id = msg_in[1] * 0xFF + msg_in[2];
            m = split_l(msg_in, '\0', 3, &match, 3, msg_in_size);
            if (msg_in_size >= 9 && m == 2 && is_username(match[0]) && is_display_name(match[1]) && is_secret(match[2])) {
                memcpy(msg_out->data.auth.username, match[0], strlen(match[0]));
                memcpy(msg_out->data.auth.display_name, match[1], strlen(match[1]));
                memcpy(msg_out->data.auth.secret, match[2], strlen(match[2]));
                free(match);
                return 0;
            }
            break;
        case e_join:
            msg_out->type = e_join;
            msg_out->id = msg_in[1] * 0xFF + msg_in[2];
            m = split_l(msg_in, '\0', 3, &match, 2, msg_in_size);
            if (msg_in_size >= 7 && m == 1 && is_channel_id(match[0]) && is_display_name(match[1])) {
                memcpy(msg_out->data.join.channel_id, match[0], strlen(match[0]));
                memcpy(msg_out->data.join.display_name, match[1], strlen(match[1]));
                free(match);
                return 0;
            }
            break;
        case e_msg:
            msg_out->type = e_msg;
            msg_out->id = msg_in[1] * 0xFF + msg_in[2];
            m = split_l(msg_in, '\0', 3, &match, 2, msg_in_size);
            if (msg_in_size >= 7 && m == 1 && is_display_name(match[0]) && is_message_content(match[1])) {
                memcpy(msg_out->data.msg.display_name, match[0], strlen(match[0]));
                memcpy(msg_out->data.msg.content, match[1], strlen(match[1]));
                free(match);
                return 0;
            }
            break;
        case e_err:
            msg_out->type = e_err;
            msg_out->id = msg_in[1] * 0xFF + msg_in[2];
            m = split_l(msg_in, '\0', 3, &match, 2, msg_in_size);
            if (msg_in_size >= 7 && m == 1 && is_display_name(match[0]) && is_message_content(match[1])) {
                memcpy(msg_out->data.msg.display_name, match[0], strlen(match[0]));
                memcpy(msg_out->data.msg.content, match[1], strlen(match[1]));
                free(match);
                return 0;
            }
            break;
        case e_bye:
            msg_out->type = e_bye;
            msg_out->id = msg_in[1] * 0xFF + msg_in[2];
            if (msg_in_size == 3) {
                free(match);
                return 0;
            }
            break;
    }

    free(match);
    return errno = error_out(error_pars_tcp_parse_fail, __LINE__, __FILE__, NULL);
}

int split_l(string_t str, char c, int s, string_t ** ptr, int max, int len) {
    int i = s;
    int j = 0;
    int m = 0;

    while (i < len && m < max) {
        if (str[i] == c) {
            j = 0;
            m++;
        } else {
            ((*ptr)[m])[j] = str[i];
            j++;
        }
        i++;
    }

    if (i != len || (i == len && m == max)) {
        m--;
    }
    return m;
}