#ifndef ENIGMA_CRACK_H
#define ENIGMA_CRACK_H

#include "enigma.h"

#define ENIGMA_PREDEFINED_PLUGBOARD_SETTINGS 1
#define ENIGMA_PREDEFINED_ROTOR_SETTINGS     2
#define ENIGMA_PREDEFINED_ROTOR_POSITIONS    4
#define ENIGMA_PREDEFINED_REFLECTOR          8
#define ENIGMA_FLAG_X_SPACED                 16

#define ENIGMA_PRINT_CONFIG(e) printf("%s %s %s %c%c%c %s %s", \
                                      e.rotors[0].name, e.rotors[1].name, e.rotors[2].name, \
                                      e.rotors[0].idx, e.rotors[1].idx, e.rotors[2].idx, \
                                      e.reflector.name, e.plugboard)

typedef struct {
    enigma_t        enigma;
    float           score;
} enigma_score_t;

typedef struct {
    enigma_t             enigma;
    int                  method;
    int                  target;
    const char*          plaintext;
    int                  plaintextPos;
    int                  plaintextLen;
    const char*          ciphertext;
    int                  ciphertextLen;
    float                minScore;
    float                maxScore;
    int                  ngramCount;
    int                  maxPlugboardSettings;
    int                  maxThreads;
    enigma_score_t*      scores;
    const char**         dictionary;
    int                  dictSize;
    int                  scoreCount;
    int                  flags;
    float*               letterFreqTargets;
    float                letterFreqOffset;
} enigma_crack_config_t;

void enigma_find_potential_indices(const char*, const char*, int*);
int enigma_dict_match(const char*, const enigma_crack_config_t*);
float enigma_freq(const char*, int);
int enigma_letter_freq(const char*, int,  float*, float);

#endif
