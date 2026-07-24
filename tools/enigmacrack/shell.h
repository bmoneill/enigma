/**
 * @file enigmacrack/shell.h
 *
 * Interactive shell for cracking Enigma machine configurations.
 */
#ifndef ENIGMA_CRACK_SHELL_H
#define ENIGMA_CRACK_SHELL_H

/* ── Attribute vary flags ───────────────────────────────────────────────── */
/** Vary the rotor placed in slot 0 (right-most). */
#define SHELL_VARY_R0 (1 << 0)
/** Vary the rotor placed in slot 1. */
#define SHELL_VARY_R1 (1 << 1)
/** Vary the rotor placed in slot 2. */
#define SHELL_VARY_R2 (1 << 2)
/** Vary the rotor placed in slot 3 (4-rotor Enigma only). */
#define SHELL_VARY_R3 (1 << 3)
/** Vary the starting position of the rotor in slot 0. */
#define SHELL_VARY_R0P (1 << 4)
/** Vary the starting position of the rotor in slot 1. */
#define SHELL_VARY_R1P (1 << 5)
/** Vary the starting position of the rotor in slot 2. */
#define SHELL_VARY_R2P (1 << 6)
/** Vary the starting position of the rotor in slot 3. */
#define SHELL_VARY_R3P (1 << 7)
/** Vary the reflector (UKW). */
#define SHELL_VARY_REFLECTOR (1 << 8)
/** Vary the plugboard (Steckerbrett). */
#define SHELL_VARY_PLUGBOARD (1 << 9)

/* ── Analysis methods ───────────────────────────────────────────────────── */
#define SHELL_METHOD_IOC   1
#define SHELL_METHOD_NGRAM 2

/* ── Display limit ──────────────────────────────────────────────────────── */
/** Maximum number of top scores shown by 'score' / 'analyze'. */
#define SHELL_DISPLAY_SCORES 10

/* ── Ciphertext buffer ──────────────────────────────────────────────────── */
#define SHELL_CIPHER_MAX 4096

/**
 * @brief Start the interactive Enigma cracking shell.
 *
 * Initialises a default Enigma configuration in memory and enters an
 * interactive command loop.  Returns 0 on clean exit.
 */
int start_shell(void);

#endif /* ENIGMA_CRACK_SHELL_H */
