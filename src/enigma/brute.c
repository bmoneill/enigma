 /**
  * @file enigma/brute.c
  *
  * This file implements scoring functions for brute force dictionary cracking
  * of Enigma machines that were hopefully already partially cracked.
  */
#include "brute.h"

#include "common.h"

EMSCRIPTEN_KEEPALIVE float enigma_brute_score(const EnigmaCrackParams* cfg, const char* text) {
    return (float) enigma_dict_match(cfg, text);
}
