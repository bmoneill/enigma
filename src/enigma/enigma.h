/**
 * @file enigma/enigma.h
 *
 * This file defines the core structures and functions for the Enigma machine,
 * including the main Enigma structure, encoding functions, and initialization
 * routines for default and random configurations.
 */
#ifndef ENIGMA_ENIGMA_H
#define ENIGMA_ENIGMA_H

#include "reflector.h"
#include "rotor.h"

#ifndef ENIGMA_VERSION
/**
 * @brief libenigma version string
 */
#define ENIGMA_VERSION "unknown"
#endif

/**
 * @struct Enigma
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
 */
typedef struct {
    const EnigmaRotor*     rotors[4]; //!< Array of pointers to rotor configurations.
    int                    rotor_indices[4]; //!< Array of current rotor indices.
    int                    rotor_flag; //!< Flag indicating rotor behavior.
    int                    rotor_count; //!< Number of rotors in use.
    const EnigmaReflector* reflector; //!< Pointer to reflector configuration.
    char                   plugboard[27]; //!< String representing plugboard settings.
} Enigma;

char        enigma_encode(Enigma*, int);
int         enigma_encode_string(Enigma*, const char*, char*, int);
int         enigma_init_rotors(Enigma*, const EnigmaRotor*, int);
int         enigma_init_default_config(Enigma*);
int         enigma_init_random_config(Enigma*);
Enigma*     enigma_new(void);
const char* enigma_version(void);

/* --- Enigma getters and setters --- */
const char*            enigma_get_plugboard(const Enigma*);
const EnigmaReflector* enigma_get_reflector(const Enigma*);
const EnigmaRotor*     enigma_get_rotor(const Enigma*, int);
int                    enigma_get_rotor_count(const Enigma*);
int                    enigma_get_rotor_flag(const Enigma*);
int                    enigma_get_rotor_index(const Enigma*, int);
int                    enigma_set_plugboard(Enigma*, const char*);
int                    enigma_set_reflector(Enigma*, int);
int                    enigma_set_rotor(Enigma*, int, int);
int                    enigma_set_rotor_count(Enigma*, int);
int                    enigma_set_rotor_flag(Enigma*, int);
int                    enigma_set_rotor_index(Enigma*, int, int);

#endif
