#include "enigma/common.h"
#include "enigma/io.h"
#include "enigma/ngram.h"
#include "unity.h"

#include "util.c"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

const char*       success = "Expected success";
const char*       failure = "Expected failure";
Enigma            enigma;
EnigmaCrackParams cfg;

void              setUp(void) { memset(&cfg, 0, sizeof(EnigmaCrackParams)); }

void              test_enigma_load_config(void) {
    const char* configStr = "I II III|XYZ|B|ABCDEF";
    int         result    = enigma_load_config(&enigma, configStr);

    TEST_ASSERT_EQUAL_INT_MESSAGE(ENIGMA_SUCCESS, result, success);
    TEST_ASSERT_EQUAL_INT_MESSAGE(3,
                                  enigma.rotor_count,
                                  "Expected rotor count to match config string");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("I",
                                     enigma.rotors[0]->name,
                                     "Expected rotor name to match config string");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("II",
                                     enigma.rotors[1]->name,
                                     "Expected rotor name to match config string");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("III",
                                     enigma.rotors[2]->name,
                                     "Expected rotor name to match config string");
    TEST_ASSERT_EQUAL_CHAR_MESSAGE('X',
                                   enigma.rotor_indices[0] + 'A',
                                   "Expected rotor index to match config string");
    TEST_ASSERT_EQUAL_CHAR_MESSAGE('Y',
                                   enigma.rotor_indices[1] + 'A',
                                   "Expected rotor index to match config string");
    TEST_ASSERT_EQUAL_CHAR_MESSAGE('Z',
                                   enigma.rotor_indices[2] + 'A',
                                   "Expected rotor index to match config string");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("B",
                                     enigma.reflector->name,
                                     "Expected reflector name to match config string");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("ABCDEF",
                                     enigma.plugboard,
                                     "Expected plugboard to match config string");
}

void test_enigma_load_config_WithLengthyString(void) {
    char* configStr = malloc(100);
    for (int i = 0; i < 99; i++) {
        configStr[i] = 'A';
    }
    configStr[99] = '\0';

    TEST_ASSERT_EQUAL_INT(ENIGMA_FAILURE, enigma_load_config(&enigma, configStr));
}

void test_enigma_load_config_WithInvalidRotors(void) {
    const char* configStr = "A AB ABC|XYZ|B|ABCDEF";

    TEST_ASSERT_EQUAL_INT(ENIGMA_FAILURE, enigma_load_config(&enigma, configStr));
}

void test_enigma_load_config_WithInvalidRotorPositions(void) {
    const char* configStr = "I II III|?!?|B|ABCDEF";

    TEST_ASSERT_EQUAL_INT(ENIGMA_FAILURE, enigma_load_config(&enigma, configStr));
}

void test_enigma_load_config_WithInvalidReflector(void) {
    const char* configStr = "I II III|XYZ|Z|ABCDEF";

    TEST_ASSERT_EQUAL_INT(ENIGMA_FAILURE, enigma_load_config(&enigma, configStr));
}

void test_enigma_load_custom_reflector(void) {
    const char*     alphabet        = "ZYXWVUTSRQPONMLKJIHGFEDCBA";
    const char*     invalidAlphabet = "ZYXWVUT";
    const char*     reflectorName   = "Custom Reflector";
    EnigmaReflector reflector;

    int             result = enigma_load_custom_reflector(&reflector, alphabet, reflectorName);
    TEST_ASSERT_EQUAL_INT_MESSAGE(ENIGMA_SUCCESS, result, success);
    for (int i = 0; i < ENIGMA_ALPHA_SIZE; i++) {
        TEST_ASSERT_EQUAL_INT_MESSAGE(25 - i,
                                      reflector.indices[i],
                                      "Expected reflector index to match config string");
    }
    TEST_ASSERT_EQUAL_STRING_MESSAGE(reflectorName,
                                     reflector.name,
                                     "Expected reflector name to match config string");

    result = enigma_load_custom_reflector(&reflector, invalidAlphabet, reflectorName);
    TEST_ASSERT_EQUAL_INT_MESSAGE(ENIGMA_FAILURE, result, failure);
}

void test_enigma_load_custom_rotor(void) {
    const char* rotorAlphabet        = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const char* invalidRotorAlphabet = "ABCDEFG";
    const char* rotorName            = "Custom Rotor";
    int         rotorNotches[2]      = { 16, 4 };
    EnigmaRotor rotor;

    int result = enigma_load_custom_rotor(&rotor, rotorAlphabet, rotorName, rotorNotches, 2);
    TEST_ASSERT_EQUAL_INT_MESSAGE(ENIGMA_SUCCESS, result, success);
    for (int i = 0; i < ENIGMA_ALPHA_SIZE; i++) {
        TEST_ASSERT_EQUAL_INT_MESSAGE(rotorAlphabet[i] - 'A',
                                      rotor.fwd_indices[i],
                                      "Expected forward index to be correct");
        TEST_ASSERT_EQUAL_INT_MESSAGE(i,
                                      rotor.rev_indices[rotor.fwd_indices[i]],
                                      "Expected reverse index to be correct");
    }
    TEST_ASSERT_EQUAL_STRING_MESSAGE(rotorName,
                                     rotor.name,
                                     "Expected reflector name to match config string");
    TEST_ASSERT_EQUAL_INT_MESSAGE(2, rotor.notches_count, "Expected notches count to match config");
    TEST_ASSERT_EQUAL_INT_MESSAGE(rotorNotches[0],
                                  rotor.notches[0],
                                  "Expected notches to match config");
    TEST_ASSERT_EQUAL_INT_MESSAGE(rotorNotches[1],
                                  rotor.notches[1],
                                  "Expected notches to match config");

    result = enigma_load_custom_rotor(&rotor, invalidRotorAlphabet, rotorName, rotorNotches, 2);
    TEST_ASSERT_EQUAL_INT_MESSAGE(ENIGMA_FAILURE, result, failure);
}

void test_enigma_load_ngrams(void) {
    // Assumes test/data/bigrams.txt contains:
    // 2 500
    // 10 TH
    // 5 CH
    // 1 EA
    // 50 HE
    // 20 AR
    int               charCount = 500;
    char              cwd[1024];
    EnigmaCrackParams cfg;

    int               result = enigma_load_ngrams(&cfg, get_path("bigrams.txt"));
    TEST_ASSERT_EQUAL_INT_MESSAGE(ENIGMA_SUCCESS, result, success);
    TEST_ASSERT_EQUAL_INT_MESSAGE(2, cfg.n, "Expected n to be 2");
    TEST_ASSERT_NOT_NULL_MESSAGE(cfg.ngrams, "Expected ngrams to be not null");
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(
        10.0 / charCount,
        cfg.ngrams[ENIGMA_BIIDX('T', 'H')],
        "Expected TH bigram to equal value set in test/data/bigrams.txt");
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(
        5.0 / charCount,
        cfg.ngrams[ENIGMA_BIIDX('C', 'H')],
        "Expected CH bigram to equal value set in test/data/bigrams.txt");
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(
        1.0 / charCount,
        cfg.ngrams[ENIGMA_BIIDX('E', 'A')],
        "Expected EA bigram to equal value set in test/data/bigrams.txt");
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(
        50.0 / charCount,
        cfg.ngrams[ENIGMA_BIIDX('H', 'E')],
        "Expected HE bigram to equal value set in test/data/bigrams.txt");
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(
        20.0 / charCount,
        cfg.ngrams[ENIGMA_BIIDX('A', 'R')],
        "Expected AR bigram to equal value set in test/data/bigrams.txt");

    free(cfg.ngrams);
}

void test_enigma_load_ngrams_WithTrigrams(void) {
    // Assumes test/data/trigrams.txt contains:
    // 3 500
    // 10 THE
    // 5 CHA
    // 1 EAC
    // 50 HER
    // 20 ART
    int               charCount = 500;
    char              cwd[1024];
    EnigmaCrackParams cfg;

    int               result = enigma_load_ngrams(&cfg, get_path("trigrams.txt"));
    TEST_ASSERT_EQUAL_INT_MESSAGE(ENIGMA_SUCCESS, result, success);
    TEST_ASSERT_EQUAL_INT_MESSAGE(3, cfg.n, "Expected n to be 3");
    TEST_ASSERT_NOT_NULL_MESSAGE(cfg.ngrams, "Expected ngrams to be not null");
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(
        10.0 / charCount,
        cfg.ngrams[ENIGMA_TRIIDX('T', 'H', 'E')],
        "Expected THE trigram to equal value set in test/data/trigrams.txt");
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(
        5.0 / charCount,
        cfg.ngrams[ENIGMA_TRIIDX('C', 'H', 'A')],
        "Expected CHA trigram to equal value set in test/data/trigrams.txt");
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(
        1.0 / charCount,
        cfg.ngrams[ENIGMA_TRIIDX('E', 'A', 'C')],
        "Expected EAC trigram to equal value set in test/data/trigrams.txt");
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(
        50.0 / charCount,
        cfg.ngrams[ENIGMA_TRIIDX('H', 'E', 'R')],
        "Expected HER trigram to equal value set in test/data/trigrams.txt");
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(
        20.0 / charCount,
        cfg.ngrams[ENIGMA_TRIIDX('A', 'R', 'T')],
        "Expected ART trigram to equal value set in test/data/trigrams.txt");

    free(cfg.ngrams);
}

void test_enigma_load_ngrams_WithQuadgrams(void) {
    // Assumes test/data/quadgrams.txt contains:
    // 4 500
    // 10 THER
    // 5 CHAN
    // 1 EACH
    // 50 HERA
    // 20 ARTI
    int               charCount = 500;
    char              cwd[1024];
    EnigmaCrackParams cfg;

    int               result = enigma_load_ngrams(&cfg, get_path("quadgrams.txt"));
    TEST_ASSERT_EQUAL_INT_MESSAGE(ENIGMA_SUCCESS, result, success);
    TEST_ASSERT_EQUAL_INT_MESSAGE(4, cfg.n, "Expected n to be 4");
    TEST_ASSERT_NOT_NULL_MESSAGE(cfg.ngrams, "Expected ngrams to be not null");
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(
        10.0 / charCount,
        cfg.ngrams[ENIGMA_QUADIDX('T', 'H', 'E', 'R')],
        "Expected THER trigram to equal value set in test/data/quadgrams.txt");
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(
        5.0 / charCount,
        cfg.ngrams[ENIGMA_QUADIDX('C', 'H', 'A', 'N')],
        "Expected CHAN trigram to equal value set in test/data/quadgrams.txt");
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(
        1.0 / charCount,
        cfg.ngrams[ENIGMA_QUADIDX('E', 'A', 'C', 'H')],
        "Expected EACH trigram to equal value set in test/data/quadgrams.txt");
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(
        50.0 / charCount,
        cfg.ngrams[ENIGMA_QUADIDX('H', 'E', 'R', 'A')],
        "Expected HERA trigram to equal value set in test/data/quadgrams.txt");
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(
        20.0 / charCount,
        cfg.ngrams[ENIGMA_QUADIDX('A', 'R', 'T', 'I')],
        "Expected ARTI trigram to equal value set in test/data/quadgrams.txt");

    free(cfg.ngrams);
}

void test_enigma_load_ngrams_WherePathIsInvalid(void) {
    EnigmaCrackParams cfg;
    int               result = enigma_load_ngrams(&cfg, "foo.txt");
    TEST_ASSERT_EQUAL_INT_MESSAGE(ENIGMA_FAILURE, result, failure);
}

void test_enigma_load_ngrams_WhereFirstLineIsInvalid(void) {
    EnigmaCrackParams cfg;
    int               result = enigma_load_ngrams(&cfg, get_path("ngrams_invalid_first_line.txt"));
    TEST_ASSERT_EQUAL_INT_MESSAGE(ENIGMA_FAILURE, result, failure);
}

void test_enigma_load_ngrams_WhereNIsInvalid(void) {
    EnigmaCrackParams cfg;
    int               result = enigma_load_ngrams(&cfg, get_path("ngrams_invalid_n.txt"));
    TEST_ASSERT_EQUAL_INT_MESSAGE(ENIGMA_FAILURE, result, failure);
}

void test_enigma_load_plugboard_config(void) {
    const char* plugboardConfig       = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const char* longPlugboardConfig   = "ABCDEFGHIJKLMNOPQRSTUVWXYZA"; // 27 characters
    const char* unevenPlugboardConfig = "ABCDE"; // Odd number of characters

    int         result                = enigma_load_plugboard_config(&enigma, plugboardConfig);
    TEST_ASSERT_EQUAL_INT_MESSAGE(ENIGMA_SUCCESS, result, success);

    result = enigma_load_plugboard_config(&enigma, longPlugboardConfig);
    TEST_ASSERT_EQUAL_INT_MESSAGE(ENIGMA_FAILURE, result, failure);

    result = enigma_load_plugboard_config(&enigma, unevenPlugboardConfig);
    TEST_ASSERT_EQUAL_INT_MESSAGE(ENIGMA_FAILURE, result, failure);
}

void test_enigma_load_reflector_config(void) {
    int result = enigma_load_reflector_config(&enigma, "B");
    TEST_ASSERT_EQUAL_INT_MESSAGE(ENIGMA_SUCCESS, result, success);
    TEST_ASSERT_EQUAL_STRING_MESSAGE("B",
                                     enigma.reflector->name,
                                     "Expected reflector to be set to UKW-B");

    for (int i = 0; i < ENIGMA_ALPHA_SIZE; i++) {
        TEST_ASSERT_EQUAL_INT_MESSAGE(enigma_reflectors[1]->indices[i],
                                      enigma.reflector->indices[i],
                                      "Expected reflector indices to match");
    }

    result = enigma_load_reflector_config(&enigma, "NonExistentReflector");
    TEST_ASSERT_EQUAL_INT_MESSAGE(ENIGMA_FAILURE, result, failure);
}

void test_enigma_load_rotor_config(void) {
    char buf[64];
    strcpy(buf, "I II III");

    int result = enigma_load_rotor_config(&enigma, buf);
    TEST_ASSERT_EQUAL_INT_MESSAGE(ENIGMA_SUCCESS, result, success);
    TEST_ASSERT_EQUAL_STRING_MESSAGE("I",
                                     enigma.rotors[0]->name,
                                     "Expected rotor to be set properly");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("II",
                                     enigma.rotors[1]->name,
                                     "Expected rotor to be set properly");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("III",
                                     enigma.rotors[2]->name,
                                     "Expected rotor to be set properly");

    result = enigma_load_rotor_config(&enigma, "NonExistentRotor");
    TEST_ASSERT_EQUAL_INT_MESSAGE(ENIGMA_FAILURE, result, failure);
}

void test_enigma_load_rotor_positions(void) {
    enigma.rotor_count = 3;

    int result         = enigma_load_rotor_positions(&enigma, "XYZ");
    TEST_ASSERT_EQUAL_INT_MESSAGE(ENIGMA_SUCCESS, result, success);
    TEST_ASSERT_EQUAL_CHAR_MESSAGE('X',
                                   enigma.rotor_indices[0] + 'A',
                                   "Expected rotor position to be set properly");
    TEST_ASSERT_EQUAL_CHAR_MESSAGE('Y',
                                   enigma.rotor_indices[1] + 'A',
                                   "Expected rotor position to be set properly");
    TEST_ASSERT_EQUAL_CHAR_MESSAGE('Z',
                                   enigma.rotor_indices[2] + 'A',
                                   "Expected rotor position to be set properly");

    result = enigma_load_rotor_positions(&enigma, "X1Z");
    TEST_ASSERT_EQUAL_INT_MESSAGE(ENIGMA_FAILURE, result, failure);
}

void test_enigma_load_rotor_positions_WhereEnigmaHasNoRotors(void) {
    enigma.rotor_count = 0;
    int result         = enigma_load_rotor_positions(&enigma, "XYZ");
    TEST_ASSERT_EQUAL_INT_MESSAGE(ENIGMA_FAILURE, result, failure);
}

void test_enigma_print_config(void) {
    char buf[128];

    enigma.rotor_count      = 3;
    enigma.rotors[0]        = enigma_rotors[0];
    enigma.rotor_indices[0] = 0;
    enigma.rotors[1]        = enigma_rotors[1];
    enigma.rotor_indices[1] = 1;
    enigma.rotors[2]        = enigma_rotors[2];
    enigma.rotor_indices[2] = 2;
    enigma.reflector        = enigma_reflectors[1];
    strcpy(enigma.plugboard, "ABCDEF");

    enigma_print_config(&enigma, buf);
    TEST_ASSERT_EQUAL_STRING_MESSAGE("I II III|ABC|B|ABCDEF",
                                     buf,
                                     "Expected configuration string to be printed properly");
}
