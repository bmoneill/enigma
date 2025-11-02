/**
 * @file enigma/crack.h
 *
 * Header file for cracking Enigma machine configurations.
 */
#ifndef ENIGMA_CRACK_H
#define ENIGMA_CRACK_H

#include "enigma.h"
#include "score.h"

#include <stddef.h>

/**
 * @struct enigma_crack_t
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
    enigma_t             enigma;
    enigma_score_list_t* score_list;
    const char**         dictionary;
    size_t               dictionary_length;
    float*               ngrams;
    int                  n;
    size_t               ngrams_length;
    const char*          ciphertext;
    size_t               ciphertext_length;
    int                  flags;
    float                frequency_targets[26];
    float                min_score;
    float                max_score;
    float                target_score;
    float                target_frequency;
    float                frequency_offset;
    const char*          known_plaintext;
    int                  known_plaintext_length;
} enigma_crack_t;

/**
 * English letter frequency targets (from
 * https://pi.math.cornell.edu/~mec/2003-2004/cryptography/subs/frequencies.html).
 *
 * X is probably unreliable here due to possibly being used as a space substitute.
 */
static const float englishFreq[26]
    = { 0.0812f, 0.0149f, 0.0271f, 0.0432f, 0.1202f, 0.023f,  0.0203f, 0.0592f, 0.0731f,
        0.0010f, 0.0069f, 0.0398f, 0.0261f, 0.0695f, 0.0768f, 0.0182f, 0.0011f, 0.0602f,
        0.0628f, 0.0910f, 0.0288f, 0.0111f, 0.0209f, 0.0017f, 0.0211f, 0.0007f };

int   enigma_crack_rotor(enigma_crack_t*, int, float (*)(const enigma_crack_t*, const char*));
int   enigma_crack_rotors(enigma_crack_t*, float (*)(const enigma_crack_t*, const char*));
int   enigma_crack_rotor_positions(enigma_crack_t*, float (*)(const enigma_crack_t*, const char*));
int   enigma_crack_reflector(enigma_crack_t*, float (*)(const enigma_crack_t*, const char*));
int   enigma_crack_plugboard(enigma_crack_t*, float (*)(const enigma_crack_t*, const char*));
int   enigma_dict_match(const enigma_crack_t*, const char*);
int   enigma_find_potential_indices(const char*, const char*, int*);
float enigma_freq(const char*, int);
int   enigma_letter_freq(const enigma_crack_t*, const char*);
int   enigma_score_append(enigma_crack_t*, enigma_t*, const char*, float);
int   enigma_score_flags(const enigma_crack_t*, const char*);

/* enigma_crack_t getters and setters */
const enigma_t*            enigma_crack_get_enigma(const enigma_crack_t*);
const enigma_score_list_t* enigma_crack_get_score_list(const enigma_crack_t*);
const char**               enigma_crack_get_dictionary(const enigma_crack_t*);
size_t                     enigma_crack_get_dictionary_length(const enigma_crack_t*);
const float*               enigma_crack_get_ngrams(const enigma_crack_t*);
int                        enigma_crack_get_n(const enigma_crack_t*);
size_t                     enigma_crack_get_ngrams_length(const enigma_crack_t*);
const char*                enigma_crack_get_ciphertext(const enigma_crack_t*);
size_t                     enigma_crack_get_ciphertext_length(const enigma_crack_t*);
int                        enigma_crack_get_flags(const enigma_crack_t*);
const float*               enigma_crack_get_frequency_targets(const enigma_crack_t*);
float                      enigma_crack_get_min_score(const enigma_crack_t*);
float                      enigma_crack_get_max_score(const enigma_crack_t*);
float                      enigma_crack_get_target_score(const enigma_crack_t*);
const char*                enigma_crack_get_known_plaintext(const enigma_crack_t*);
size_t                     enigma_crack_get_known_plaintext_length(const enigma_crack_t*);
int                        enigma_crack_set_enigma(enigma_crack_t*, enigma_t*);
int                        enigma_crack_set_score_list(enigma_crack_t*, enigma_score_list_t*);
int                        enigma_crack_set_dictionary(enigma_crack_t*, const char**, size_t);
int                        enigma_crack_set_ngrams(enigma_crack_t*, float*, int, size_t);
int                        enigma_crack_set_n(enigma_crack_t*, int);
int                        enigma_crack_set_ciphertext(enigma_crack_t*, const char*, size_t);
int                        enigma_crack_set_flags(enigma_crack_t*, int);
int                        enigma_crack_set_frequency_targets(enigma_crack_t*, float*);
int                        enigma_crack_set_min_score(enigma_crack_t*, float);
int                        enigma_crack_set_max_score(enigma_crack_t*, float);
int                        enigma_crack_set_target_score(enigma_crack_t*, float);
int                        enigma_crack_set_known_plaintext(enigma_crack_t*, const char*, size_t);

#endif
