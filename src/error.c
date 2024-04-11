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
    }
#endif
    return errno;
}

char * _get_error_msg(error_code errno) {
    switch (errno) {
        case error_argv_malformed_argv: return "Malformed server arguments";
        default: return "Unknown error";
    }
}