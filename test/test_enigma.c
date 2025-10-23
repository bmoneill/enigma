#include "unity.h"
#include "enigma/enigma.c"

void setUp(void) { }

void tearDown(void) { }

void test_enigma_encode(void) {
    enigma_t enigma;
    enigma_init_random_config(&enigma);

    int idx = enigma.rotor_indices[0];

    char encoded = enigma_encode(&enigma, 'A');
    TEST_ASSERT_TRUE(encoded >= 'A' && encoded <= 'Z');
    TEST_ASSERT_NOT_EQUAL('A', encoded);
    TEST_ASSERT_EQUAL_INT(idx + 1 >= ENIGMA_ALPHA_SIZE ? 0 : idx + 1, enigma.rotor_indices[0]);
}

void test_enigma_encode_string(void) {
    enigma_t enigma;
    enigma_init_default_config(&enigma);

    const char* input = "HELLO";
    char output[6] = {0};

    enigma_encode_string(&enigma, input, output, strlen(input));

    TEST_ASSERT_EQUAL_STRING("ILBDA", output);
}

void test_enigma_init_rotors(void) {
    enigma_t enigma;
    const enigma_rotor_t rotor_array[3] = {
        enigma_rotor_III,
        enigma_rotor_II,
        enigma_rotor_I
    };

    enigma_init_rotors(&enigma, rotor_array, 3);

    TEST_ASSERT_EQUAL_STRING("III", enigma.rotors[0]->name);
    TEST_ASSERT_EQUAL_STRING("II", enigma.rotors[1]->name);
    TEST_ASSERT_EQUAL_STRING("I", enigma.rotors[2]->name);
    TEST_ASSERT_EQUAL_INT(3, enigma.rotor_count);
}

void test_enigma_init_default_config(void) {
    enigma_t enigma;
    enigma_init_default_config(&enigma);

    TEST_ASSERT_EQUAL_INT(3, enigma.rotor_count);
    TEST_ASSERT_EQUAL_STRING("III", enigma.rotors[0]->name);
    TEST_ASSERT_EQUAL_STRING("II", enigma.rotors[1]->name);
    TEST_ASSERT_EQUAL_STRING("I", enigma.rotors[2]->name);
}

void test_enigma_init_random_config(void) {
    enigma_t enigma;
    enigma_init_random_config(&enigma);

    TEST_ASSERT_TRUE(enigma.rotor_count == 3 || enigma.rotor_count == 4);
    for (int i = 0; i < enigma.rotor_count; i++) {
        TEST_ASSERT_TRUE(enigma.rotor_indices[i] >= 0 && enigma.rotor_indices[i] < ENIGMA_ALPHA_SIZE);
    }
    for (size_t i = 0; i < strlen(enigma.plugboard); i++) {
        TEST_ASSERT_TRUE(enigma.plugboard[i] >= 'A' && enigma.plugboard[i] <= 'Z');
    }
    TEST_ASSERT_NOT_NULL(enigma.reflector->name);
}

void test_rotate(void) {
    int idx = 0;
    rotate(&idx);
    TEST_ASSERT_EQUAL_INT(1, idx);

    idx = ENIGMA_ALPHA_SIZE - 1;
    rotate(&idx);
    TEST_ASSERT_EQUAL_INT(0, idx);
}

void test_rotate_rotors(void) {
    enigma_t enigma;
    enigma_rotor_t rotor1 = enigma_rotor_I;
    enigma_rotor_t rotor2 = enigma_rotor_II;
    enigma_rotor_t rotor3 = enigma_rotor_III;

    enigma_init_default_config(&enigma);
    enigma.rotor_count = 3;
    enigma.rotors[0] = &rotor1;
    enigma.rotors[1] = &rotor2;
    enigma.rotors[2] = &rotor3;

    enigma.rotor_indices[0] = 0;
    rotate_rotors(&enigma);

    TEST_ASSERT_EQUAL_INT(1, enigma.rotor_indices[0]); // First rotor rotated
    TEST_ASSERT_EQUAL_INT(0, enigma.rotor_indices[1]);  // Second rotor not rotated

    enigma.rotor_indices[0] = 9;
    enigma.rotor_indices[1] = 17;
    rotor2.fwd_indices[17] = 25;
    rotor2.notches[0] = 25;
    rotor2.numNotches = 1;
    rotate_rotors(&enigma);

    TEST_ASSERT_EQUAL_INT(10, enigma.rotor_indices[0]); // First rotor rotated
    TEST_ASSERT_EQUAL_INT(18, enigma.rotor_indices[1]);  // Second rotor rotated
}

void test_rotor_pass_forward(void) {
    int idx = 0;
    enigma_rotor_t rotor = {
        .fwd_indices = {4, 10, 12, 5, 11, 6, 3, 16, 21, 25,
                        13, 19, 14, 22, 24, 7, 23, 20, 18, 15,
                        0, 8, 1, 17, 2, 9},
    };

    TEST_ASSERT_EQUAL_INT(4, rotor_pass_forward(&rotor, idx, 0)); // A -> E
    TEST_ASSERT_EQUAL_INT(10, rotor_pass_forward(&rotor, idx, 1)); // B -> K
}

void test_rotor_pass_reverse(void) {
    int idx = 0;
    enigma_rotor_t rotor = {
        .rev_indices = {20, 22, 24, 6, 0, 3, 5, 15, 21, 25,
                        1, 4, 2, 10, 12, 19, 7, 23, 18, 11,
                        17, 8, 16, 14, 13, 9},
    };

    TEST_ASSERT_EQUAL_INT(0, rotor_pass_reverse(&rotor, idx, 4)); // E -> A
    TEST_ASSERT_EQUAL_INT(1, rotor_pass_reverse(&rotor, idx, 10)); // K -> B
}

void test_substitute(void) {
    enigma_t enigma;
    strcpy(enigma.plugboard, "ABCD");

    TEST_ASSERT_EQUAL_CHAR('B', substitute(enigma.plugboard, 'A'));
    TEST_ASSERT_EQUAL_CHAR('A', substitute(enigma.plugboard, 'B'));

    TEST_ASSERT_EQUAL_CHAR('D', substitute(enigma.plugboard, 'C'));
    TEST_ASSERT_EQUAL_CHAR('C', substitute(enigma.plugboard, 'D'));

    TEST_ASSERT_EQUAL_CHAR('E', substitute(enigma.plugboard, 'E'));
}
