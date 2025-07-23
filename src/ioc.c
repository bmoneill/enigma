#include "ioc.h"

#include "enigma.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void crack_plugboard(enigma_t *, const char *, char *, int, float);
static void crack_reflector(enigma_t *, const char *, char *, int, float);
static void crack_rotor_indexes(enigma_t *, const char *, char *, int, float);
static void crack_rotor_positions(enigma_t *, const char*, char*, int, float);
static void encode_string(enigma_t*, const char*, char*, int);
static void find_freq(int *, const char*, int len);
static float find_ic(float, int*, int);

const float enigma_ic_english_min = 0.05769; // 1.5/26
const float enigma_ic_english_max = 0.07692; // 2.0/26

float enigma_ic_score(const char *text, int len, void *placeholder) {
    int freq[26] = {0};
    float total = 0.0f;

    for (int i = 0; i < len; i++) {
        if (text[i] < 'A' || text[i] > 'Z') {
            continue;
        }
        freq[text[i] - 'A']++;
    }

    for (int i = 0; i < 26; i++) {
        total += (float)freq[i] * (freq[i] - 1);
    }
    return total / (float)(len * (len - 1));
}

