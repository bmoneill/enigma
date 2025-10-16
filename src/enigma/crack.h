/**
 * @file enigma/crack.h
 *
 * Header file for cracking Enigma machine configurations.
 */
#ifndef ENIGMA_CRACK_H
#define ENIGMA_CRACK_H

#include "enigma.h"

#define ENIGMA_FLAG_DICTIONARY_MATCH         1
#define ENIGMA_FLAG_FREQUENCY                2

#define ENIGMA_PRINT_CONFIG(e) printf("%s %s %s %c%c%c %s %s", \
                                      e.rotors[0].name, e.rotors[1].name, e.rotors[2].name, \
                                      e.rotors[0].idx, e.rotors[1].idx, e.rotors[2].idx, \
                                      e.reflector.name, e.plugboard);


typedef struct {
    char ngram[5];
    float score;
    int count;
} enigma_ngram_t;

typedef struct {
    enigma_ngram_t* ngrams;
    int count;
    int n;
} enigma_ngram_list_t;

typedef struct {
    enigma_t        enigma;
    float           score;
} enigma_score_t;

typedef struct {
    enigma_score_t*       scores;
    int                   scoreCount;
    int                   maxScores;
} enigma_score_list_t;

typedef struct {
    enigma_ngram_list_t* ngrams;
    enigma_score_list_t* scores;
    const char**         dictionary;
    int                  dictSize;
    int                  frequencies;
    enigma_t             enigma;
    const char*          ciphertext;
    int                  ciphertextLen;
    float                minScore;
    float                maxScore;
    float                targetScore;
    int                  flags;
} enigma_crack_config_t;

void enigma_crack_rotor            (enigma_crack_config_t*,  int,
                                    float (*)(const char*, const enigma_crack_config_t*));
void enigma_crack_rotors           (enigma_crack_config_t*,  float (*)(const char*, const enigma_crack_config_t*));
void enigma_crack_rotor_positions  (enigma_crack_config_t*,  float (*)(const char*, const enigma_crack_config_t*));
void enigma_crack_reflector        (enigma_crack_config_t*,  float (*)(const char*, const enigma_crack_config_t*));
void enigma_crack_plugboard        (enigma_crack_config_t*,  float (*)(const char*, const enigma_crack_config_t*));

void  enigma_find_potential_indices(const char*,                    const char*,                  int*);
int   enigma_dict_match            (const char*,                    const enigma_crack_config_t*);
float enigma_freq                  (const char*,                    int);
int   enigma_letter_freq           (const char*,                    int,                          float*,
                                    float,                          float*);
void  enigma_score_append          (enigma_score_list_t*,           float);
void  enigma_score_print           (const enigma_score_list_t*);
void  enigma_score_sort            (enigma_score_list_t*);

#endif
