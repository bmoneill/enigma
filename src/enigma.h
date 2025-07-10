#ifndef ENIGMA_H
#define ENIGMA_H

typedef struct {
    char *name;
    char alphabet[26];
    char *notches;
    char turnover;
    int position;
} rotor_t;

typedef struct {
    char *name;
    rotor_t *rotors[3];
    char reflector[26];
    char *plugboard[26];
} enigma_t;

char encode(enigma_t*, char);

#endif
