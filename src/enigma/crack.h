/**
 * @file enigma/crack.h
 *
 * Header file for cracking Enigma machine configurations.
 */
#ifndef LIBENIGMA_CRACK_H
#define LIBENIGMA_CRACK_H

#include "enigma.h"
#include "score.h"

#include <stddef.h>

/**
 * @struct EnigmaCrackParams
 * @brief A structure representing a configuration for cracking an Enigma cipher.
 *
 * @param enigma            The base Enigma machine configuration.
 * @param score_list            A list of scored Enigma configurations.
 * @param dictionary        An array of dictionary words.
 * @param dictionary_length The number of words in the dictionary.
 * @param ngrams            An array of n-gram frequencies.
 * @param n                 The length of each n-gram.
 * @param ngrams_length     The number of n-grams.
 * @param ciphertext        The ciphertext to be cracked.
 * @param ciphertext_length The length of the ciphertext.
 * @param flags             Flags indicating special conditions a scored configuration may meet.
 * @param frequency_targets An array of frequency targets for each letter.
 * @param min_score         The minimum score for a configuration to be considered valid.
 * @param max_score         The maximum score for a configuration to be considered valid.
 * @param target_score      The target score for a configuration to be considered valid.
 * @param target_frequency  The target frequency for a configuration to be considered valid.
 * @param frequency_offset  The maximum offset from the target frequency that a scored configuration
 *                          may have to be considered valid.
 * @param known_plaintext   Known plaintext.
 */
typedef struct {
    Enigma           enigma;
    EnigmaScoreList* score_list;
    const char**     dictionary;
    size_t           dictionary_length;
    float*           ngrams;
    int              n;
    size_t           ngrams_length;
    const char*      ciphertext;
    size_t           ciphertext_length;
    int              flags;
    float            frequency_targets[26];
    float            min_score;
    float            max_score;
    float            target_score;
    float            target_frequency;
    float            frequency_offset;
    const char*      known_plaintext;
    int              known_plaintext_length;
} EnigmaCrackParams;

EnigmaCrackParams* enigma_crack_params_new(void);
int   enigma_crack_rotor(EnigmaCrackParams*, int, float (*)(const EnigmaCrackParams*, const char*));
int   enigma_crack_rotors(EnigmaCrackParams*, float (*)(const EnigmaCrackParams*, const char*));
int   enigma_crack_rotor_positions(EnigmaCrackParams*,
                                   float (*)(const EnigmaCrackParams*, const char*));
int   enigma_crack_reflector(EnigmaCrackParams*, float (*)(const EnigmaCrackParams*, const char*));
int   enigma_crack_plugboard(EnigmaCrackParams*, float (*)(const EnigmaCrackParams*, const char*));
int   enigma_dict_match(const EnigmaCrackParams*, const char*);
int   enigma_find_potential_indices(const char*, const char*, int*);
float enigma_freq(const char*, int);
int   enigma_letter_freq(const EnigmaCrackParams*, const char*);
int   enigma_score_append(EnigmaCrackParams*, Enigma*, const char*, float);
int   enigma_score_flags(const EnigmaCrackParams*, const char*);

/* EnigmaCrackParams getters and setters */
const Enigma*          enigma_crack_get_enigma(const EnigmaCrackParams*);
const EnigmaScoreList* enigma_crack_get_score_list(const EnigmaCrackParams*);
const char**           enigma_crack_get_dictionary(const EnigmaCrackParams*);
size_t                 enigma_crack_get_dictionary_length(const EnigmaCrackParams*);
const float*           enigma_crack_get_ngrams(const EnigmaCrackParams*);
int                    enigma_crack_get_n(const EnigmaCrackParams*);
size_t                 enigma_crack_get_ngrams_length(const EnigmaCrackParams*);
const char*            enigma_crack_get_ciphertext(const EnigmaCrackParams*);
size_t                 enigma_crack_get_ciphertext_length(const EnigmaCrackParams*);
int                    enigma_crack_get_flags(const EnigmaCrackParams*);
const float*           enigma_crack_get_frequency_targets(const EnigmaCrackParams*);
float                  enigma_crack_get_min_score(const EnigmaCrackParams*);
float                  enigma_crack_get_max_score(const EnigmaCrackParams*);
float                  enigma_crack_get_target_score(const EnigmaCrackParams*);
const char*            enigma_crack_get_known_plaintext(const EnigmaCrackParams*);
size_t                 enigma_crack_get_known_plaintext_length(const EnigmaCrackParams*);
int                    enigma_crack_set_enigma(EnigmaCrackParams*, Enigma*);
int                    enigma_crack_set_score_list(EnigmaCrackParams*, EnigmaScoreList*);
int                    enigma_crack_set_dictionary(EnigmaCrackParams*, const char**, size_t);
int                    enigma_crack_set_ngrams(EnigmaCrackParams*, float*, int, size_t);
int                    enigma_crack_set_n(EnigmaCrackParams*, int);
int                    enigma_crack_set_ciphertext(EnigmaCrackParams*, const char*, size_t);
int                    enigma_crack_set_flags(EnigmaCrackParams*, int);
int                    enigma_crack_set_frequency_targets(EnigmaCrackParams*, float*);
int                    enigma_crack_set_min_score(EnigmaCrackParams*, float);
int                    enigma_crack_set_max_score(EnigmaCrackParams*, float);
int                    enigma_crack_set_target_score(EnigmaCrackParams*, float);
int                    enigma_crack_set_known_plaintext(EnigmaCrackParams*, const char*, size_t);

#endif
