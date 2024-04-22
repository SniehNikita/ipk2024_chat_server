/**
 * @file error.h
 * @author Sniehovskyi Nikita (xsnieh00)
 * @date 11.04.2024
 * @brief Error codes translation and error logging.
 */

#ifndef __ERROR_H__

#define __ERROR_H__

#include <stdlib.h>
#include <stdio.h>

#include "types.h"
#include "error.h"

typedef enum {
    error_argv_malformed_argv = 1,
    error_memo_mem_alloc_fail = 2,
    error_fatl_client_missing = 20,
    error_serv_sock_init_fail = 101,
    error_serv_addr_bind_fail = 102,
    error_serv_tcp_listn_fail = 103,
    error_serv_tcp_accpt_fail = 104,
    error_pars_tcp_parse_fail = 201,
    error_pars_tcp_no_crlf_ch = 202,
    error_pars_udp_parse_fail = 203,
    error_comp_tcp_no_confirm = 301
} error_code;

int error_out(error_code errno, int lineno, char *file_name, char *msg);

char * _get_error_msg(error_code errno);

#endif