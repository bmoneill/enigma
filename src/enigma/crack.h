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
#define ENIGMA_FLAG_KNOWN_PLAINTEXT          4

#define ENIGMA_PRINT_CONFIG(e) printf("%s %s %s %c%c%c %s %s", \
                                      e.rotors[0].name, e.rotors[1].name, e.rotors[2].name, \
                                      e.rotors[0].idx, e.rotors[1].idx, e.rotors[2].idx, \
                                      e.reflector.name, e.plugboard);


typedef struct {
    enigma_t        enigma;
    float           score;
    int             flags;
} enigma_score_t;

typedef struct {
    enigma_score_t*       scores;
    int                   scoreCount;
    int                   maxScores;
} enigma_score_list_t;

typedef struct {
    enigma_score_list_t* scores;
    const char**         dictionary;
    int                  dictSize;
    int                  frequencies;
    float* ngrams;
    int n;
    int ngramLen;
    enigma_t             enigma;
    const char*          ciphertext;
    int                  ciphertextLen;
    float                minScore;
    float                maxScore;
    float                targetScore;
    float                minFreq;
    float                maxFreq;
    float*               freqTargets;
    const char*          plaintext;
    int                  flags;
} enigma_crack_config_t;

// English letter frequency targets (from https://pi.math.cornell.edu/~mec/2003-2004/cryptography/subs/frequencies.html)
// X is probably unreliable here due to possibly being used as a space substitute
static const float englishFreq[26] = {
    0.0812f, 0.0149f, 0.0271f, 0.0432f, 0.1202f, 0.023f, 0.0203f, 0.0592f, 0.0731f, 0.0010f,
    0.0069f, 0.0398f, 0.0261f, 0.0695f, 0.0768f, 0.0182f, 0.0011f, 0.0602f, 0.0628f, 0.0910f,
    0.0288f, 0.0111f, 0.0209f, 0.0017f, 0.0211f, 0.0007f
};

// TODO score functions should probably have arguments switched
void  enigma_crack_rotor           (enigma_crack_config_t*,                               int,
                                    float (*)(const char*, const enigma_crack_config_t*));
void  enigma_crack_rotors          (enigma_crack_config_t*,                               float (*)(const char*, const enigma_crack_config_t*));
void  enigma_crack_rotor_positions (enigma_crack_config_t*,                               float (*)(const char*, const enigma_crack_config_t*));
void  enigma_crack_reflector       (enigma_crack_config_t*,                               float (*)(const char*, const enigma_crack_config_t*));
void  enigma_crack_plugboard       (enigma_crack_config_t*,                               float (*)(const char*, const enigma_crack_config_t*));
int   enigma_dict_match            (const char*,                                          const enigma_crack_config_t*);
void  enigma_find_potential_indices(const char*,                                          const char*,
                                    int*);
float enigma_freq                  (const char*,                                          int);
int   enigma_letter_freq           (const char*,                                          const enigma_crack_config_t*);
void  enigma_score_append          (enigma_crack_config_t*,                               enigma_t*,
                                    const char*,                                          float);
int   enigma_score_flags           (const char*,                                          const enigma_crack_config_t*);
void  enigma_score_print           (const enigma_score_list_t*);
void  enigma_score_sort            (enigma_score_list_t*);

#endif
