#include "unity.h"
#include "enigma/io.h"

#include <stdlib.h>
#include <string.h>

void setUp(void) { }

void tearDown(void) { }

void test_enigma_load_config(void) {
    const char* configStr = "I II III|XYZ|B|ABCDEF";
    enigma_t enigma;
    int result = enigma_load_config(&enigma, configStr);

    TEST_ASSERT_EQUAL_INT(0, result);
    TEST_ASSERT_EQUAL_INT(3, enigma.rotor_count);
    TEST_ASSERT_EQUAL_STRING("I", enigma.rotors[0]->name);
    TEST_ASSERT_EQUAL_STRING("II", enigma.rotors[1]->name);
    TEST_ASSERT_EQUAL_STRING("III", enigma.rotors[2]->name);
    TEST_ASSERT_EQUAL_CHAR('X', enigma.rotor_indices[0] + 'A');
    TEST_ASSERT_EQUAL_CHAR('Y', enigma.rotor_indices[1] + 'A');
    TEST_ASSERT_EQUAL_CHAR('Z', enigma.rotor_indices[2] + 'A');
    TEST_ASSERT_EQUAL_STRING("B", enigma.reflector->name);
    TEST_ASSERT_EQUAL_STRING("ABCDEF", enigma.plugboard);
}

void test_enigma_load_custom_reflector(void) {
    const char* alphabet = "ZYXWVUTSRQPONMLKJIHGFEDCBA";
    const char* invalidAlphabet = "ZYXWVUT";
    const char* reflectorName = "Custom Reflector";
    enigma_reflector_t reflector;

    int result = enigma_load_custom_reflector(&reflector, alphabet, reflectorName);
    TEST_ASSERT_EQUAL_INT(0, result);
    for (int i = 0; i < ENIGMA_ALPHA_SIZE; i++) {
        TEST_ASSERT_EQUAL_INT(25 - i, reflector.indices[i]);
    }
    TEST_ASSERT_EQUAL_STRING(reflectorName, reflector.name);

    result = enigma_load_custom_reflector(&reflector, invalidAlphabet, reflectorName);
    TEST_ASSERT_NOT_EQUAL(0, result);
}

void test_enigma_load_custom_rotor(void) {
    const char* rotorAlphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const char* invalidRotorAlphabet = "ABCDEFG";
    const char* rotorName = "Custom Rotor";
    int rotorNotches[2] = { 16, 4 };
    enigma_rotor_t rotor;

    int result = enigma_load_custom_rotor(&rotor, rotorAlphabet, rotorName, rotorNotches, 2);
    TEST_ASSERT_EQUAL_INT(0, result);
    for (int i = 0; i < ENIGMA_ALPHA_SIZE; i++) {
        TEST_ASSERT_EQUAL_INT(rotorAlphabet[i] - 'A', rotor.fwd_indices[i]);
        TEST_ASSERT_EQUAL_INT(i, rotor.rev_indices[rotor.fwd_indices[i]]);
    }
    TEST_ASSERT_EQUAL_STRING(rotorName, rotor.name);
    TEST_ASSERT_EQUAL_INT(2, rotor.numNotches);
    TEST_ASSERT_EQUAL_INT(rotorNotches[0], rotor.notches[0]);
    TEST_ASSERT_EQUAL_INT(rotorNotches[1], rotor.notches[1]);

    result = enigma_load_custom_rotor(&rotor, invalidRotorAlphabet, rotorName, rotorNotches, 2);
    TEST_ASSERT_NOT_EQUAL(0, result);
}

void test_load_ngrams(void) {
    // Assumes test/files/bigrams.txt contains:
    // 2 500
    // 10 TH
    // 5 CH
    // 1 EA
    // 50 HE
    // 20 AR

    int charCount = 500;

    const char* ngramFilePath = "data/bigrams.txt";
    enigma_crack_config_t cfg;

    int result = enigma_load_ngrams(&cfg, ngramFilePath);
    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_EQUAL_INT(2, cfg.n);
    TEST_ASSERT_NOT_NULL(cfg.ngrams);
    TEST_ASSERT_EQUAL_FLOAT(10.0/charCount, cfg.ngrams[ENIGMA_BIIDX('T', 'H')]);
    TEST_ASSERT_EQUAL_FLOAT(5.0/charCount, cfg.ngrams[ENIGMA_BIIDX('C', 'H')]);
    TEST_ASSERT_EQUAL_FLOAT(1.0/charCount, cfg.ngrams[ENIGMA_BIIDX('E', 'A')]);
    TEST_ASSERT_EQUAL_FLOAT(50.0/charCount, cfg.ngrams[ENIGMA_BIIDX('H', 'E')]);
    TEST_ASSERT_EQUAL_FLOAT(20.0/charCount, cfg.ngrams[ENIGMA_BIIDX('A', 'R')]);

    free(cfg.ngrams);
}

void test_enigma_load_plugboard_config(void) {
    const char* plugboardConfig = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const char* longPlugboardConfig = "ABCDEFGHIJKLMNOPQRSTUVWXYZA"; // 27 characters
    const char* unevenPlugboardConfig = "ABCDE"; // Odd number of characters
    enigma_t enigma;

    int result = enigma_load_plugboard_config(&enigma, plugboardConfig);
    TEST_ASSERT_EQUAL(0, result);

    result = enigma_load_plugboard_config(&enigma, longPlugboardConfig);
    TEST_ASSERT_NOT_EQUAL(0, result);

    result = enigma_load_plugboard_config(&enigma, unevenPlugboardConfig);
    TEST_ASSERT_NOT_EQUAL(0, result);
}

void test_enigma_load_reflector_config(void) {
    enigma_t enigma;

    int result = enigma_load_reflector_config(&enigma, "B");
    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_EQUAL_STRING("B", enigma.reflector->name);

    for (int i = 0; i < ENIGMA_ALPHA_SIZE; i++) {
        TEST_ASSERT_EQUAL_INT(enigma_reflectors[1]->indices[i], enigma.reflector->indices[i]);
    }

    result = enigma_load_reflector_config(&enigma, "NonExistentReflector");
    TEST_ASSERT_NOT_EQUAL(0, result);
}

void test_enigma_load_rotor_config(void) {
    enigma_t enigma;
    char buf[64];
    strcpy(buf, "I II III");

    int result = enigma_load_rotor_config(&enigma, buf);
    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_EQUAL_STRING("I", enigma.rotors[0]->name);
    TEST_ASSERT_EQUAL_STRING("II", enigma.rotors[1]->name);
    TEST_ASSERT_EQUAL_STRING("III", enigma.rotors[2]->name);

    result = enigma_load_rotor_config(&enigma, "NonExistentRotor");
    TEST_ASSERT_NOT_EQUAL(0, result);
}

void test_enigma_load_rotor_positions(void) {
    enigma_t enigma;
    enigma.rotor_count = 3;

    int result = enigma_load_rotor_positions(&enigma, "XYZ");
    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_EQUAL_CHAR('X', enigma.rotor_indices[0] + 'A');
    TEST_ASSERT_EQUAL_CHAR('Y', enigma.rotor_indices[1] + 'A');
    TEST_ASSERT_EQUAL_CHAR('Z', enigma.rotor_indices[2] + 'A');

    result = enigma_load_rotor_positions(&enigma, "X1Z");
    TEST_ASSERT_NOT_EQUAL(0, result);
}

void test_enigma_print_config(void) {
    enigma_t enigma;
    char buf[128];

    enigma.rotor_count = 3;
    enigma.rotors[0] = enigma_rotors[0];
    enigma.rotor_indices[0] = 0;
    enigma.rotors[1] = enigma_rotors[1];
    enigma.rotor_indices[1] = 1;
    enigma.rotors[2] = enigma_rotors[2];
    enigma.rotor_indices[2] = 2;
    enigma.reflector = enigma_reflectors[1];
    strcpy(enigma.plugboard, "ABCDEF");

    enigma_print_config(&enigma, buf);

    TEST_ASSERT_EQUAL_STRING("I II III|ABC|B|ABCDEF", buf);
}
