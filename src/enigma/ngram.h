/**
 * @file enigma/ngram.h
 *
 * This file declares functions for cracking Enigma configurations
 * using n-gram scoring.
 */
#ifndef ENIGMA_NGRAM_H
#define ENIGMA_NGRAM_H

#include "crack.h"

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

void  enigma_crack_rotors_ngram         (const enigma_crack_config_t*, const enigma_ngram_list_t*);
void  enigma_crack_rotor_positions_ngram(const enigma_crack_config_t*, const enigma_ngram_list_t*);
void  enigma_crack_reflector_ngram      (const enigma_crack_config_t*, const enigma_ngram_list_t*);
void  enigma_crack_plugboard_ngram      (const enigma_crack_config_t*, const enigma_ngram_list_t*);

float enigma_bigram_score               (const char*,                  const enigma_crack_config_t*, const float*);
float enigma_trigram_score              (const char*,                  const enigma_crack_config_t*, const float*);
float enigma_quadram_score              (const char*,                  const enigma_crack_config_t*, const float*);
void  enigma_ngram_analyze              (const char*,                  const enigma_crack_config_t*, const enigma_ngram_list_t*)

#endif
