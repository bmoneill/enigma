#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ioc.h"

static void print_usage(const char* argv0);

int main(int argc, char* argv[]) {

    return EXIT_SUCCESS;
}

/**
 * @brief Print usage information for the Bombe CLI.
 *
 * This function prints the command line options and their descriptions.
 *
 * @param argv0 The name of the program, typically `argv[0]`.
 *
 */
static void print_usage(const char* argv0) {
    fprintf(stderr, "Usage: %s [-t threadCount] ciphertext\n", argv0);
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  -t threadCount Set the number of threads to use\n");
    exit(EXIT_FAILURE);
}
