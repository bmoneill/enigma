#include "ngram.h"

#include <stdio.h>
#include <stdlib.h>

#define DEFAULT_NGRAM_COUNT 100000

#define BIIDX(a, b) ((a << 5) | b)
#define TRIIDX(a, b, c) ((a << 10) | (b << 5) | c)
#define QUADIDX(a, b, c, d) ((a << 15) | (b << 10) | (c << 5) | d)

typedef struct {
    char ngram[5];
    float score;
    int count;
} ngram_t;

typedef struct {
    ngram_t *ngrams;
    int count;
    int n;
} ngram_list_t;

ngram_list_t *enigma_init_ngrams(const char *path) {
    char line[16];
    int lineCount = 0;
    int reallocCount = 0;
    FILE *f = fopen(path, "r");
    if (!f) {
        fprintf(stderr, "Failed to open ngram file: %s\n", path);
    }

    ngram_list_t *ngram_list = malloc(sizeof(ngram_list_t));
    ngram_list->ngrams = malloc(DEFAULT_NGRAM_COUNT * sizeof(ngram_t));
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
    } else {
        fprintf(stderr, "Failed to read ngram file: %s\n", path);
        free(ngram_list->ngrams);
        free(ngram_list);
        fclose(f);
        return NULL;
    }

    while (fgets(line, sizeof(line), f)) {
        if (ngram_list->count >= DEFAULT_NGRAM_COUNT * (reallocCount + 1)) {
            ngram_list->ngrams = realloc(ngram_list->ngrams, (ngram_list->count + DEFAULT_NGRAM_COUNT * ++reallocCount) * sizeof(ngram_t));
        }
        if (sscanf(line, "%d %4s", &ngram_list->ngrams[ngram_list->count].count, ngram_list->ngrams[ngram_list->count].ngram) == 2) {
            ngram_list->count++;
        }
    }

    fclose(f);
    return ngram_list;
}

float enigma_bigram_score(const char *text, int textLen, const float *bigrams) {
    float total = 0.0f;
    int next = text[0] - 'A';

    for (int i = 1; i < textLen; i++) {
        int cur = next;
        next = text[i] - 'A';
        if (cur < 0 || cur >= 26 || next < 0 || next >= 26) {
            continue;
        }
        total += bigrams[BIIDX(cur, next)];
    }

    return total;
}

float enigma_trigram_score(const char *text, int textLen, const float *trigrams) {
    float total = 0.0f;
    int next1 = text[0] - 'A';
    int next2 = text[1] - 'A';

    for (int i = 2; i < textLen; i++) {
        int cur = next1;
        next1 = next2;
        next2 = text[i] - 'A';
        if (cur < 0 || cur >= 26 || next1 < 0 || next1 >= 26 || next2 < 0 || next2 >= 26) {
            continue;
        }
        total += trigrams[TRIIDX(cur, next1, next2)];
    }

    return total;
}

float enigma_quadram_score(const char *text, int textLen, const float *quadgrams) {
    float total = 0.0f;
    int next1 = text[0] - 'A';
    int next2 = text[1] - 'A';
    int next3 = text[2] - 'A';

    for (int i = 3; i < textLen; i++) {
        int cur = next1;
        next1 = next2;
        next2 = next3;
        next3 = text[i] - 'A';
        if (cur < 0 || cur >= 26 || next1 < 0 || next1 >= 26 || next2 < 0 || next2 >= 26 || next3 < 0 || next3 >= 26) {
            continue;
        }
        total += quadgrams[QUADIDX(cur, next1, next2, next3)];
    }

    return total;
}
