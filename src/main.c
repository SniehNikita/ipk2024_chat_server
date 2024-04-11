/**
 * @file main.c
 * @author Sniehovskyi Nikita (xsnieh00)
 * @date 11.04.2024
 * @brief Root program logic.
 */

#include "main.h"

int main(int argc, char **argv_in) {
    parse_argv(argc, argv_in, &argv);

    if (argv.is_help) {
        printf("%s\n", help_msg);
        return 0;
    }
    
    return 0;
}