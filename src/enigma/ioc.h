#ifndef ENIGMA_IC_H
#define ENIGMA_IC_H

#include "crack.h"

static const float enigma_ioc_english_min = 0.05769; // 1.5/26
static const float enigma_ioc_english_max = 0.07692; // 2.0/26

float enigma_ioc_score(const char*, int, void*);
void enigma_crack_rotors_ioc(enigma_crack_config_t*);
void enigma_crack_rotor_positions_ioc(enigma_crack_config_t*);
void enigma_crack_reflector_ioc(enigma_crack_config_t*);
void enigma_crack_plugboard_ioc(enigma_crack_config_t*);

#endif
