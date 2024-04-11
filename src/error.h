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

#define DEBUG

typedef enum {
    error_argv_malformed_argv = 1
} error_code;

int error_out(error_code errno, int lineno, char *file_name, char *msg);

char * _get_error_msg(error_code errno);

#endif