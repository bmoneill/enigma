/**
 * @file enigma/enigma.h
 *
 * This file defines the core structures and functions for the Enigma machine,
 * including the main Enigma structure, encoding functions, and initialization
 * routines for default and random configurations.
 */
#ifndef ENIGMA_H
#define ENIGMA_H

#include "reflectors.h"
#include "rotors.h"

/**
 * @struct enigma_t
 * @brief Represents the state and configuration of an Enigma machine.
 *
 * This structure holds the rotors, reflector, and plugboard settings
 * for an Enigma machine. It supports up to 4 rotors and includes fields
 * for the rotor count and a flag for rotor behavior.
 *
 * The plugboard is represented as a string of character pairs, where each
 * pair indicates two letters that are swapped during encoding.
 *
 * Example plugboard configuration: "ABCD" means A<->B and C<->D are swapped.
 *
 * Note: The reflector and rotors are represented using their respective structures
 * defined in `reflectors.h` and `rotors.h`.
 *
 * @param rotors Array of pointers to rotor configurations.
 * @param rotor_indices Array of current rotor indices.
 * @param rotor_flag Flag indicating rotor behavior.
 * @param rotor_count Number of rotors in use.
 * @param reflector Pointer to reflector configuration.
 * @param plugboard String representing plugboard settings.
 */
typedef struct {
    const enigma_rotor_t*     rotors[4];
    int                       rotor_indices[4];
    int                       rotor_flag;
    int                       rotor_count;
    const enigma_reflector_t* reflector;
    char                      plugboard[27];
} enigma_t;

char enigma_encode(enigma_t*, int);
void enigma_encode_string(enigma_t*, const char*, char*, int);
void enigma_init_rotors(enigma_t*, const enigma_rotor_t*, int);
void enigma_init_default_config(enigma_t*);
void enigma_init_random_config(enigma_t*);
int  enigma_set_plugboard(enigma_t*, const char*);
int  enigma_set_reflector(enigma_t*, int);
int  enigma_set_rotor(enigma_t*, int, int);
int  enigma_set_rotor_count(enigma_t*, int);
int  enigma_set_rotor_index(enigma_t*, int, int);

#endif
