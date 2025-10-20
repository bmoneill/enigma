#include "unity.h"
#include "enigma/enigma.c"

void setUp(void) { }

void tearDown(void) { }

void test_rotate_rotors(void) {
    enigma_t enigma;
    enigma_init_default_config(&enigma);
    enigma.rotor_count = 3;

    enigma.rotors[0].idx = 0;
    rotate_rotors(&enigma);

    TEST_ASSERT_EQUAL_INT(1, enigma.rotors[0].idx); // First rotor rotated
    TEST_ASSERT_EQUAL_INT(0, enigma.rotors[1].idx);  // Second rotor not rotated

    enigma.rotors[0].idx = 9;
    enigma.rotors[1].idx = 17;
    enigma.rotors[1].fwd_indices[17] = 25;
    enigma.rotors[1].notches[0] = 25;
    enigma.rotors[1].numNotches = 1;
    rotate_rotors(&enigma);

    TEST_ASSERT_EQUAL_INT(10, enigma.rotors[0].idx); // First rotor rotated
    TEST_ASSERT_EQUAL_INT(18, enigma.rotors[1].idx);  // Second rotor rotated
}

void test_rotor_pass_forward(void) {
    enigma_rotor_t rotor = {
        .fwd_indices = {4, 10, 12, 5, 11, 6, 3, 16, 21, 25,
                        13, 19, 14, 22, 24, 7, 23, 20, 18, 15,
                        0, 8, 1, 17, 2, 9},
        .idx = 0
    };

    TEST_ASSERT_EQUAL_INT(4, rotor_pass_forward(&rotor, 0)); // A -> E
    TEST_ASSERT_EQUAL_INT(10, rotor_pass_forward(&rotor, 1)); // B -> K
}

void test_rotor_pass_reverse(void) {
    enigma_rotor_t rotor = {
        .rev_indices = {20, 22, 24, 6, 0, 3, 5, 15, 21, 25,
                        1, 4, 2, 10, 12, 19, 7, 23, 18, 11,
                        17, 8, 16, 14, 13, 9},
        .idx = 0
    };

    TEST_ASSERT_EQUAL_INT(0, rotor_pass_reverse(&rotor, 4)); // E -> A
    TEST_ASSERT_EQUAL_INT(1, rotor_pass_reverse(&rotor, 10)); // K -> B
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

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_rotate_rotors);
    RUN_TEST(test_rotor_pass_forward);
    RUN_TEST(test_rotor_pass_reverse);
    RUN_TEST(test_substitute);

    return UNITY_END();
}
