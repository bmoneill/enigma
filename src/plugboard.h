#ifndef PLUGBOARD_H
#define PLUGBOARD_H

typedef struct {
    const char *name;
    const char *wiring; // Even length string representing pairs of letters
} plugboard_t;

char plugboard_substitute(plugboard_t *, char);

#endif