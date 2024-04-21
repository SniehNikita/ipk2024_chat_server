/**
 * @file error.c
 * @author Sniehovskyi Nikita (xsnieh00)
 * @date 11.04.2024
 * @brief Error codes translation and error logging.
 */

#include "error.h"

int error_out(error_code errno, int lineno, char *file_name, char *msg) {
#ifdef DEBUG
    fprintf(stderr, "Error [%d] on line [%d]", errno, lineno);
    if (file_name != NULL) {
        fprintf(stderr, " in file [%s]: ", file_name);
    } else {
        fprintf(stderr, ": ");
    }
    fprintf(stderr, "%s", _get_error_msg(errno));
    if (msg != NULL) {
        fprintf(stderr, ". With message [%s].\n", msg);
    } else {
        fprintf(stderr, "\n");
    }
#endif
    return errno;
}

char * _get_error_msg(error_code errno) {
    switch (errno) {
        case error_argv_malformed_argv: return "Malformed server arguments";
        case error_memo_mem_alloc_fail: return "Memory allocation failed";
        case error_fatl_client_missing: return "One of the clients is missing...";
        case error_serv_sock_init_fail: return "Socket creation failed";
        case error_serv_addr_bind_fail: return "Address binding failed";
        case error_serv_tcp_listn_fail: return "Listening on TCP socket failed";
        case error_serv_tcp_accpt_fail: return "Accept on TCP socket failed";
        case error_pars_tcp_parse_fail: return "TCP message parse failed";
        case error_pars_tcp_no_crlf_ch: return "Message doesn't end with \\r\\n";
        case error_comp_tcp_no_confirm: return "Confirm is not part of TCP";
        case error_pars_udp_parse_fail: return "UDP message parse failed";
        // case : return "";
        // case : return "";
        // case : return "";
        // case : return "";
        // case : return "";
        // case : return "";
        // case : return "";
        // case : return "";
        // case : return "";
        // case : return "";
        // case : return "";
        // case : return "";
        // case : return "";
        // case : return "";
        // case : return "";
        // case : return "";
        // case : return "";
        // case : return "";
        // case : return "";
        // case : return "";
        // case : return "";
        // case : return "";
        // case : return "";
        // case : return "";
        default: return "Unknown error";
    }
}