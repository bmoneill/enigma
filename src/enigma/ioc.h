#ifndef ENIGMA_IC_H
#define ENIGMA_IC_H

#include "crack.h"

// Average English index of coincidence is 1.73/26
static const float enigma_ioc_english_min = 0.05769; // 1.5/26
static const float enigma_ioc_english_max = 0.07692; // 2.0/26

// Average German index of coincidence is 2.05/26
static const float enigma_ioc_german_min = 0.06730; // 1.75/26
static const float enigma_ioc_german_max = 0.08654; // 2.25/26

float enigma_ioc_score(const char*, const enigma_crack_config_t*);
void enigma_crack_rotors_ioc(enigma_crack_config_t*);
void enigma_crack_rotor_positions_ioc(enigma_crack_config_t*);
void enigma_crack_reflector_ioc(enigma_crack_config_t*);
void enigma_crack_plugboard_ioc(enigma_crack_config_t*);

#endif
