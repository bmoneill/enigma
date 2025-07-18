#ifndef ENIGMA_H
#define ENIGMA_H

#define ALPHA_SIZE 26
#define ROTOR_COUNT 8
#define REFLECTOR_COUNT 3

typedef struct {
    const char* name;
    int indices[ALPHA_SIZE];
} reflector_t;

typedef struct {
    const char* name;
    int fwd_indices[ALPHA_SIZE];
    int rev_indices[ALPHA_SIZE];
    int notches[2];
    int idx;
} rotor_t;

typedef struct {
    rotor_t rotors[4];
    int rotor_flag;
    int rotor_count;
    reflector_t* reflector;
    const char* plugboard;
} enigma_t;

char encode(enigma_t*, char);
void deinit_enigma(enigma_t*);
void init_rotors(enigma_t*, const rotor_t*, int);
void init_default_enigma(enigma_t*);

#endif
