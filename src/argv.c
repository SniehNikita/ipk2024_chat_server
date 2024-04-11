/**
 * @file argv.c
 * @author Sniehovskyi Nikita (xsnieh00)
 * @date 11.04.2024
 * @brief Argument parsing.
 */

#include "argv.h"

int parse_argv(int argc, char **argv_in, argv_t * argv_out) {
    int i = 1;

    // Filling default values
    strcpy(argv_out->ipv4, "0.0.0.0");
    argv_out->port = 4567;
    argv_out->is_help = false;
    argv_out->udp_timeout = 250;
    argv_out->udp_retransmissions = 3;

    while(i < argc) {
        if (!strcmp(argv_in[i], "-h")) {
            argv_out->is_help = true;
        } else if (!strcmp(argv_in[i], "-l") && i + 1 < argc && strlen(argv_in[i]) <= IPV4_MAX_LENGTH) {
            strcpy(argv_out->ipv4, argv_in[++i]);
        } else if (!strcmp(argv_in[i], "-p") && i + 1 < argc) {
            argv_out->port = atoi(argv_in[++i]);
        } else if (!strcmp(argv_in[i], "-d") && i + 1 < argc) {
            argv_out->udp_timeout = atoll(argv_in[++i]);
        } else if (!strcmp(argv_in[i], "-r") && i + 1 < argc) {
            argv_out->udp_retransmissions = atoi(argv_in[++i]);
        } else {
            return errno = error_out(error_argv_malformed_argv, __LINE__, __FILE__, NULL);
        }
        i++;
    }

    return 0;
}