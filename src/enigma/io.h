/**
 * @file enigma/io.h
 *
 * This file declares input/output functions.
 */
#ifndef ENIGMA_IO_H
#define ENIGMA_IO_H

#include "crack.h"
#include "enigma.h"

/**
 * @brief Prints an error message to stderr with function name.
 */
#define ENIGMA_ERROR(fmt, ...) enigma_error_message(__func__, fmt, __VA_ARGS__)

/**
 * @brief Generic invalid argument error message.
 */
static const char* enigma_invalid_argument_message = "Invalid argument provided.";

int                enigma_error_message(const char*, const char*, ...);
int                enigma_load_config(Enigma*, const char*);
int                enigma_load_custom_reflector(EnigmaReflector*, const char*, const char*);
int                enigma_load_custom_rotor(EnigmaRotor*, const char*, const char*, int*, int);
int                enigma_load_ngrams(EnigmaCrackParams*, const char*);
int                enigma_load_plugboard_config(Enigma*, const char*);
int                enigma_load_reflector_config(Enigma*, const char*);
int                enigma_load_rotor_config(Enigma*, char*);
int                enigma_load_rotor_positions(Enigma*, const char*);
void               enigma_print_config(const Enigma*, char*);

#endif
