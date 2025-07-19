#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bombe.h"

static void print_usage(const char* argv0);

int main(int argc, char* argv[]) {
    char* crib = NULL;
    int idx = -1;
    char* ciphertext = NULL;
    int threadCount = 1;
    int indices[MAX_CRIB_INDICES] = {0};
    bombe_t bombe;

    // Parse command line options
    int opt;
    while ((opt = getopt(argc, argv, "c:i:t:")) != -1) {
        switch (opt) {
        case 'c': crib = optarg; break;
        case 'C': ciphertext = optarg; break;
        case 'i': idx = atoi(optarg); break;
        case 't': threadCount = atoi(optarg); break;
        default: print_usage(argv[0]);
        }
    }


    if (optind == argc) {
        print_usage(argv[0]);
    }

    ciphertext = argv[argc-1];

    if (!crib || !ciphertext) {
        fprintf(stderr, "Error: Crib, index, and ciphertext must be provided.\n\n");
        print_usage(argv[0]);
    }

    if (idx < 0) {
        bombe_find_potential_indices(ciphertext, crib, indices);
        for (int i = 0; indices[i] != -1; i++) {
            printf("Trying index %d...\n", indices[i]);
            bombe_init(&bombe, crib, indices[i]);
            bombe_run(&bombe, ciphertext, threadCount);
        }
    } else {
        bombe_init(&bombe, crib, idx);
        bombe_run(&bombe, ciphertext, threadCount);
    }

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
