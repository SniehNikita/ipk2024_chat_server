/**
 * @file argv.h
 * @author Sniehovskyi Nikita (xsnieh00)
 * @date 11.04.2024
 * @brief Argument parsing.
 */

#ifndef __ARGV_H__

#define __ARGV_H__

#include <stdlib.h>
#include <stdio.h>

#include "types.h"
#include "error.h"

/**
 * @brief Described in 'main.h' 
 */
extern int errno;

/**
 * @brief Parses argc number of arguments from argv_in into argv_out
 * 
 * @param argc Number of arguments in argv_in
 * @param argv_in Array of input arguments
 * @param argv_out Structure of arguments retrieved from argv_in
 * @return int Result code
 */
int parse_argv(int argc, char **argv_in, argv_t * argv_out);

#endif