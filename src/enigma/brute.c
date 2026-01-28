/**
  * @file enigma/brute.c
  *
  * This file implements scoring functions for brute force dictionary cracking
  * of Enigma machines that were hopefully already partially cracked.
  */
#include "brute.h"

#include "common.h"

/**
 * @brief Calculate the score of a given text using the brute force dictionary cracking method.
 *
 * @param cfg The configuration parameters for the Enigma machine.
 * @param text The text to be scored.
 * @return The score of the text. It will be 1.0 if two dictionary words exist in the plaintext,
 *         and 0.0 otherwise.
 */
EMSCRIPTEN_KEEPALIVE float enigma_brute_score(const EnigmaCrackParams* cfg, const char* text) {
    return (float) enigma_dict_match(cfg, text);
}
