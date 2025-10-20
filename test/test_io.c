#include "unity.h"
#include "enigma/io.h"

void setUp(void) { }

void tearDown(void) { }

void test_enigma_load_config_s(void) {
    const char* configStr = "I II III|XYZ|B|ABCDEF";
    enigma_t enigma;
    int result = enigma_load_config_s(&enigma, configStr);

    TEST_ASSERT_EQUAL_INT(0, result);
    TEST_ASSERT_EQUAL_INT(3, enigma.rotor_count);
    TEST_ASSERT_EQUAL_STRING("I", enigma.rotors[0].name);
    TEST_ASSERT_EQUAL_STRING("II", enigma.rotors[1].name);
    TEST_ASSERT_EQUAL_STRING("III", enigma.rotors[2].name);
    TEST_ASSERT_EQUAL_CHAR('X', enigma.rotors[0].idx + 'A');
    TEST_ASSERT_EQUAL_CHAR('Y', enigma.rotors[1].idx + 'A');
    TEST_ASSERT_EQUAL_CHAR('Z', enigma.rotors[2].idx + 'A');
    TEST_ASSERT_EQUAL_STRING("B", enigma.reflector.name);
    TEST_ASSERT_EQUAL_STRING("ABCDEF", enigma.plugboard);
}

void test_enigma_load_custom_reflector(void) {
    const char* reflectorIndices = "ZYXWVUTSRQPONMLKJIHGFEDCBA";
    const char* reflectorName = "Custom Reflector";
    enigma_reflector_t reflector;

    int result = enigma_load_custom_reflector(&reflector, reflectorIndices, reflectorName);

    TEST_ASSERT_EQUAL_INT(0, result);
    for (int i = 0; i < ENIGMA_ALPHA_SIZE; i++) {
        TEST_ASSERT_EQUAL_INT(25 - i, reflector.indices[i]);
    }
    TEST_ASSERT_EQUAL_STRING(reflectorName, reflector.name);
}

void test_enigma_load_custom_rotor(void) {
    const char* rotorAlphabet = "EKMFLGDQVZNTOWYHXUSPAIBRCJ";
    const char* rotorName = "Custom Rotor";
    int rotorNotches[2] = { 16, 4 };
    enigma_rotor_t rotor;

    int result = enigma_load_custom_rotor(&rotor, rotorAlphabet, rotorName, rotorNotches, 2);

    TEST_ASSERT_EQUAL_INT(0, result);
    for (int i = 0; i < ENIGMA_ALPHA_SIZE; i++) {
        TEST_ASSERT_EQUAL_INT(rotorAlphabet[i] - 'A', rotor.fwd_indices[i]);
    }
    TEST_ASSERT_EQUAL_STRING(rotorName, rotor.name);
    TEST_ASSERT_EQUAL_INT(2, rotor.numNotches);
    TEST_ASSERT_EQUAL_INT(rotorNotches[0], rotor.notches[0]);
    TEST_ASSERT_EQUAL_INT(rotorNotches[1], rotor.notches[1]);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_enigma_load_config_s);
    RUN_TEST(test_enigma_load_custom_reflector);
    RUN_TEST(test_enigma_load_custom_rotor);

    return UNITY_END();
}