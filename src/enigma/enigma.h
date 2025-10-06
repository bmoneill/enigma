#ifndef ENIGMA_H
#define ENIGMA_H

#include "reflectors.h"
#include "rotors.h"


/**
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
 */
typedef struct {
    enigma_rotor_t     rotors[4];
    int                rotor_flag;
    int                rotor_count;
    enigma_reflector_t reflector;
    char*              plugboard;
} enigma_t;

char enigma_encode             (enigma_t*, int);
void enigma_encode_string      (enigma_t*, const char*,           char*, int);
void enigma_init_rotors        (enigma_t*, const enigma_rotor_t*, int);
void enigma_init_default_config(enigma_t*);
void enigma_init_random_config (enigma_t*);

#endif
