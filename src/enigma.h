#ifndef ENIGMA_H
#define ENIGMA_H

typedef struct {
    const char *name;
    const char *alphabet;
} reflector_t;

typedef struct {
    const char *name;
    char *alphabet;
    const char *notches;
} rotor_t;

typedef struct {
    char *name;
    rotor_t *rotors;
    int rotor_count;
    reflector_t *reflector;
    const char *plugboard;
} enigma_t;

char encode(enigma_t*, char);
void init_rotors(enigma_t *, rotor_t *, int);

#endif
