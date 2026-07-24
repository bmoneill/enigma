/**
 * @file enigmacrack/repl.h
 *
 * Interactive REPL for cracking Enigma machine configurations.
 */
#ifndef ENIGMA_CRACK_REPL_H
#define ENIGMA_CRACK_REPL_H

/* ── Attribute vary flags ───────────────────────────────────────────────── */
/** Vary the rotor placed in slot 0 (right-most). */
#define REPL_VARY_R0        (1 << 0)
/** Vary the rotor placed in slot 1. */
#define REPL_VARY_R1        (1 << 1)
/** Vary the rotor placed in slot 2. */
#define REPL_VARY_R2        (1 << 2)
/** Vary the rotor placed in slot 3 (4-rotor Enigma only). */
#define REPL_VARY_R3        (1 << 3)
/** Vary the starting position of the rotor in slot 0. */
#define REPL_VARY_R0P       (1 << 4)
/** Vary the starting position of the rotor in slot 1. */
#define REPL_VARY_R1P       (1 << 5)
/** Vary the starting position of the rotor in slot 2. */
#define REPL_VARY_R2P       (1 << 6)
/** Vary the starting position of the rotor in slot 3. */
#define REPL_VARY_R3P       (1 << 7)
/** Vary the reflector (UKW). */
#define REPL_VARY_REFLECTOR (1 << 8)
/** Vary the plugboard (Steckerbrett). */
#define REPL_VARY_PLUGBOARD (1 << 9)

/* ── Analysis methods ───────────────────────────────────────────────────── */
#define REPL_METHOD_IOC   1
#define REPL_METHOD_NGRAM 2

/* ── Display limit ──────────────────────────────────────────────────────── */
/** Maximum number of top scores shown by 'score' / 'analyze'. */
#define REPL_DISPLAY_SCORES 10

/* ── Ciphertext buffer ──────────────────────────────────────────────────── */
#define REPL_CIPHER_MAX 4096

/**
 * @brief Start the interactive Enigma cracking REPL.
 *
 * Initialises a default Enigma configuration in memory and enters an
 * interactive command loop.  Returns 0 on clean exit.
 */
int start_repl(void);

#endif /* ENIGMA_CRACK_REPL_H */
