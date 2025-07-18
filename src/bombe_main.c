#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lib/bombe.h"

static void print_usage(const char* argv0);

int main(int argc, char* argv[]) {
    char* crib = NULL;
    int idx = -1;
    char* ciphertext = NULL;
    int threadCount = 1;

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

    if (!crib || !ciphertext || idx < 0) {
        fprintf(stderr, "Error: Crib, index, and ciphertext must be provided.\n\n");
        print_usage(argv[0]);
    }

    bombe_t bombe;
    bombe_init(&bombe, crib, idx);
    bombe_run(&bombe, ciphertext, threadCount);

    return EXIT_SUCCESS;
}

static void print_usage(const char* argv0) {
    fprintf(stderr, "Usage: %s [-i index] [-t threadCount] -c crib ciphertext\n", argv0);
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  -c crib       Set the crib string to use\n");
    fprintf(stderr, "  -C ciphertext Set the ciphertext to analyze\n");
    fprintf(stderr, "  -i index      Set the index where the crib string should be in the ciphertext\n");
    fprintf(stderr, "  -t threadCount Set the number of threads to use\n");
    exit(EXIT_FAILURE);
}
