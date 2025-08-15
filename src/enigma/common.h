#ifndef ENIGMA_COMMON_H
#define ENIGMA_COMMON_H

#include "enigma.h"

int load_reflector_config(enigma_t*, const char*);
int load_rotor_config(enigma_t*, char*);
int load_rotor_positions(enigma_t*, char*);

#endif
