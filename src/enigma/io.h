#ifndef ENIGMA_IO_H
#define ENIGMA_IO_H

#include "enigma.h"
#include "ngram.h"

/* CLI parameter stuff */
int enigma_load_plugboard_config(enigma_t*, const char*);
int enigma_load_reflector_config(enigma_t*, const char*);
int enigma_load_rotor_config(enigma_t*, char*);
int enigma_load_rotor_positions(enigma_t*, const char*);

/* File I/O */
enigma_ngram_list_t* enigma_load_ngrams(const char*);
int enigma_load_config(enigma_t*, const char*);
int enigma_save_config(enigma_t*, const char*);

/* Other stuff */
int enigma_load_custom_reflector(enigma_reflector_t*, const char*, const char*);
int enigma_load_custom_rotor(enigma_rotor_t*, const char*, const char*, int*, int);
void enigma_print_config(enigma_t*);

#endif
