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
 * @struct EnigmaCrackParams
 * @brief A structure representing a configuration for cracking an Enigma cipher.
 */
typedef struct {
    Enigma           enigma; //!< The base enigma machine configuration
    EnigmaScoreList* score_list; //!< A list of scored configurations
    const char**     dictionary; //!< A list of dictionary words
    size_t           dictionary_length; //!< The number of words in the dictionary
    float*           ngrams; //!< An array of n-gram frequencies
    int              n; //!< The length of each n-gram
    size_t           ngrams_length; //!< The number of n-grams in the array
    const char*      ciphertext; //!< The ciphertext to be cracked
    size_t           ciphertext_length; //!< The length of the ciphertext
    int              flags; //!< Flags indicating special conditions a scored configuration may meet
    float            frequency_targets[26]; //!< An array of frequency targets for each letter
    float            min_score; //!< The minimum score for a configuration to be considered
    float            max_score; //!< The maximum score for a configuration to be considered
    float            target_score; //!< The target score for a configuration to be considered
    float            target_frequency; //!< The target frequency for a configuration to be considered
    float            frequency_offset; //!< The maximum offset from the target frequency that a scored configuration may have to be considered valid.
    const char*      known_plaintext; //!< Known plaintext that must exist for a configuration to be considered
    int              known_plaintext_length; //!< The length of the known plaintext
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

/* --- EnigmaCrackParams getters and setters --- */
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
