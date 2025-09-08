#include "io.h"

#include <stdio.h>
#include <stdlib.h>

enigma_ngram_list_t* enigma_init_ngrams(const char* path) {
    char line[16];
    int lineCount = 0;
    int reallocCount = 0;
    FILE* f = fopen(path, "r");
    if (!f) {
        fprintf(stderr, "Failed to open ngram file: %s\n", path);
    }

    enigma_ngram_list_t* ngram_list = malloc(sizeof(enigma_ngram_list_t));
    ngram_list->ngrams = malloc(ENIGMA_DEFAULT_NGRAM_COUNT * sizeof(enigma_ngram_t));
    ngram_list->count = 0;
    ngram_list->n = -1;

    if (fgets(line, sizeof(line), f)) {
        // First line should be n value and line count of original text
        if (sscanf(line, "%d %d", &ngram_list->n, &lineCount) != 2) {
            fprintf(stderr, "Invalid ngram file format: %s\n", path);
            free(ngram_list->ngrams);
            free(ngram_list);
            fclose(f);
            return NULL;
        }
    }
    else {
        fprintf(stderr, "Failed to read ngram file: %s\n", path);
        free(ngram_list->ngrams);
        free(ngram_list);
        fclose(f);
        return NULL;
    }

    while (fgets(line, sizeof(line), f)) {
        if (ngram_list->count >= ENIGMA_DEFAULT_NGRAM_COUNT * (reallocCount + 1)) {
            ngram_list->ngrams = realloc(ngram_list->ngrams,
                                         (ngram_list->count + ENIGMA_DEFAULT_NGRAM_COUNT
                                          * ++reallocCount) * sizeof(enigma_ngram_t));
        }
        if (sscanf(line, "%d %4s", &ngram_list->ngrams[ngram_list->count].count, ngram_list->ngrams[ngram_list->count].ngram) == 2) {
            ngram_list->count++;
        }
    }

    fclose(f);
    return ngram_list;
}
