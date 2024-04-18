/**
 * @file types.c
 * @author Sniehovskyi Nikita (xsnieh00)
 * @date 11.04.2024
 * @brief Definition of all custom types used in server and type checks.
 */

#include "types.h"

bool is_message_id(string_t str) {
    regex_t regex;

    regcomp(&regex, "^[0-9]\\+$", 0);

    if (!regexec(&regex, str, 0, NULL, 0)) {
        return true;
    }
    
    return false;
}

bool is_username(string_t str) {
    regex_t regex;

    regcomp(&regex, "^[[:alnum:]-]\\{1,21\\}$", 0);

    if (!regexec(&regex, str, 0, NULL, 0)) {
        return true;
    }
    
    return false;
}

bool is_channel_id(string_t str) {
    regex_t regex;

    regcomp(&regex, "^[[:alnum:]-]\\{1,21\\}$", 0);

    if (!regexec(&regex, str, 0, NULL, 0)) {
        return true;
    }
    
    return false;
}

bool is_secret(string_t str) {
    regex_t regex;

    regcomp(&regex, "^[[:alnum:]-]\\{1,129\\}$", 0);

    if (!regexec(&regex, str, 0, NULL, 0)) {
        return true;
    }
    
    return false;
}

bool is_display_name(string_t str) {
    regex_t regex;

    regcomp(&regex, "^[[:graph:]]\\{1,21\\}$", 0);

    if (!regexec(&regex, str, 0, NULL, 0)) {
        return true;
    }
    
    return false;
}

bool is_message_content(string_t str) {
    regex_t regex;

    regcomp(&regex, "^[[:print:]]\\{1,1401\\}$", 0);

    if (!regexec(&regex, str, 0, NULL, 0)) {
        return true;
    }
    
    return false;
}