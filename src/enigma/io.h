/**
 * @file enigma/io.h
 *
 * This file declares input/output functions.
 */
#ifndef ENIGMA_IO_H
#define ENIGMA_IO_H

#include "crack.h"
#include "enigma.h"
#include "ngram.h"

int  enigma_load_config(enigma_t*, const char*);
int  enigma_load_custom_reflector(enigma_reflector_t*, const char*, const char*);
int  enigma_load_custom_rotor(enigma_rotor_t*, const char*, const char*, int*, int);
int  enigma_load_ngrams(enigma_crack_config_t*, const char*);
int  enigma_load_plugboard_config(enigma_t*, const char*);
int  enigma_load_reflector_config(enigma_t*, const char*);
int  enigma_load_rotor_config(enigma_t*, char*);
int  enigma_load_rotor_positions(enigma_t*, const char*);
void enigma_print_config(const enigma_t*, char*);

#endif
