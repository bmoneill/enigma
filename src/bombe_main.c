#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bombe.h"

static void print_usage(const char *argv0);

int main(int argc, char *argv[]) {
    char *crib = NULL;
    int idx = 0;
    char *ciphertext = NULL;

    // Parse command line options
    // Again need better names here
    int opt;
    while ((opt = getopt(argc, argv, "c:C:i:")) != -1) {
        switch (opt) {
        case 'c': crib = optarg; break;
        case 'C': ciphertext = optarg; break;
        case 'i': idx = atoi(optarg); break;
        default: print_usage(argv[0]);
        }
    }

    if (!crib || !ciphertext) {
        fprintf(stderr, "Error: Crib, index, and ciphertext must be provided.\n\n");
        print_usage(argv[0]);
    }

    bombe_t bombe;
    bombe_init(&bombe, (char *[]){crib}, (int []){idx}, 1);
    bombe_run(&bombe, ciphertext);

    return EXIT_SUCCESS;
}

static void print_usage(const char *argv0) {
    fprintf(stderr, "Usage: %s -c crib -i index -p ciphertext\n", argv0);
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  -c crib       Set the crib string to use\n");
    fprintf(stderr, "  -i index      Set the index where the crib string should be in the ciphertext\n");
    fprintf(stderr, "  -p ciphertext Set the ciphertext to analyze\n");
    exit(EXIT_FAILURE);
}
