#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ic.h"

static void print_usage(const char* argv0);

int main(int argc, char* argv[]) {
    enigma_crack("ILBDAAMTAZ", 10, enigma_ic_english);

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
    fprintf(stderr, "Usage: %s [-i index] [-t threadCount] -c crib ciphertext\n", argv0);
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  -c crib       Set the crib string to use\n");
    fprintf(stderr, "  -C ciphertext Set the ciphertext to analyze\n");
    fprintf(stderr, "  -i index      Set the index where the crib string should be in the ciphertext\n");
    fprintf(stderr, "  -t threadCount Set the number of threads to use\n");
    exit(EXIT_FAILURE);
}
