#include "enigma/enigma.c"
#include "enigma/enigma.h"
#include "unity.h"

enigma_t    enigma;
const char* success = "Expected success";
const char* failure = "Expected failure";

void        setUp(void) {
    memset(&enigma, 0, sizeof(enigma));
    enigma_init_random_config(&enigma);
}

void test_enigma_encode(void) {
    int  idx     = enigma.rotor_indices[0];

    char encoded = enigma_encode(&enigma, 'A');
    TEST_ASSERT_TRUE_MESSAGE(encoded >= 'A' && encoded <= 'Z',
                             "Expected encoded character to be an uppercase letter");
    TEST_ASSERT_NOT_EQUAL_MESSAGE('A',
                                  encoded,
                                  "Expected encoded character to not match the input character");
    TEST_ASSERT_EQUAL_INT_MESSAGE(idx + 1 >= ENIGMA_ALPHA_SIZE ? 0 : idx + 1,
                                  enigma.rotor_indices[0],
                                  "Expected rotor index to be incremented");
}

void test_enigma_encode_string(void) {
    enigma_t    enigma;
    const char* input     = "HELLO";
    char        output[6] = { 0 };

    enigma_init_default_config(&enigma);
    int ret = enigma_encode_string(&enigma, input, output, strlen(input));

    TEST_ASSERT_EQUAL_INT_MESSAGE(ENIGMA_SUCCESS, ret, success);
    TEST_ASSERT_EQUAL_STRING_MESSAGE("ILBDA", output, "Expected encoded string to match");
}

void test_enigma_encode_string_WithInvalidArguments(void) {
    char s[2];
    TEST_ASSERT_EQUAL_INT_MESSAGE(ENIGMA_FAILURE,
                                  enigma_encode_string(NULL, NULL, NULL, 1),
                                  failure);
    TEST_ASSERT_EQUAL_INT_MESSAGE(ENIGMA_FAILURE, enigma_encode_string(NULL, NULL, s, 1), failure);
    TEST_ASSERT_EQUAL_INT_MESSAGE(ENIGMA_FAILURE, enigma_encode_string(NULL, s, NULL, 1), failure);
    TEST_ASSERT_EQUAL_INT_MESSAGE(ENIGMA_FAILURE, enigma_encode_string(NULL, s, s, 1), failure);
    TEST_ASSERT_EQUAL_INT_MESSAGE(ENIGMA_FAILURE,
                                  enigma_encode_string(&enigma, NULL, NULL, 1),
                                  failure);
    TEST_ASSERT_EQUAL_INT_MESSAGE(ENIGMA_FAILURE,
                                  enigma_encode_string(&enigma, NULL, s, 1),
                                  failure);
    TEST_ASSERT_EQUAL_INT_MESSAGE(ENIGMA_FAILURE,
                                  enigma_encode_string(&enigma, s, NULL, 1),
                                  failure);
}

void test_enigma_init_rotors(void) {
    enigma_t             enigma;
    const enigma_rotor_t rotor_array[3] = { enigma_rotor_III, enigma_rotor_II, enigma_rotor_I };

    int                  ret            = enigma_init_rotors(&enigma, rotor_array, 3);

    TEST_ASSERT_EQUAL_INT_MESSAGE(ENIGMA_SUCCESS, ret, success);
    TEST_ASSERT_EQUAL_STRING_MESSAGE("III",
                                     enigma.rotors[0]->name,
                                     "Expected rotors to be set properly");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("II",
                                     enigma.rotors[1]->name,
                                     "Expected rotors to be set properly");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("I",
                                     enigma.rotors[2]->name,
                                     "Expected rotors to be set properly");
    TEST_ASSERT_EQUAL_INT_MESSAGE(3, enigma.rotor_count, "Expected rotor count to be set properly");
}

void test_enigma_init_rotors_WithInvalidArguments(void) {
    const enigma_rotor_t rotor_array[3] = { enigma_rotor_III, enigma_rotor_II, enigma_rotor_I };

    TEST_ASSERT_EQUAL_INT_MESSAGE(ENIGMA_FAILURE, enigma_init_rotors(NULL, NULL, 2), failure);
    TEST_ASSERT_EQUAL_INT_MESSAGE(ENIGMA_FAILURE,
                                  enigma_init_rotors(NULL, rotor_array, 2),
                                  failure);
    TEST_ASSERT_EQUAL_INT_MESSAGE(ENIGMA_FAILURE, enigma_init_rotors(&enigma, NULL, 2), failure);
    TEST_ASSERT_EQUAL_INT_MESSAGE(ENIGMA_FAILURE,
                                  enigma_init_rotors(&enigma, rotor_array, 0),
                                  failure);
    TEST_ASSERT_EQUAL_INT_MESSAGE(ENIGMA_FAILURE,
                                  enigma_init_rotors(&enigma, rotor_array, 5),
                                  failure);
}

void test_enigma_init_default_config(void) {
    enigma_t enigma;
    enigma_init_default_config(&enigma);

    TEST_ASSERT_EQUAL_INT_MESSAGE(3, enigma.rotor_count, "Expected rotor count to be set properly");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("III",
                                     enigma.rotors[0]->name,
                                     "Expected rotor to be set properly");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("II",
                                     enigma.rotors[1]->name,
                                     "Expected rotor to be set properly");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("I",
                                     enigma.rotors[2]->name,
                                     "Expected rotor to be set properly");
}

void test_enigma_init_default_config_WithInvalidArguments(void) {
    int ret = enigma_init_default_config(NULL);
    TEST_ASSERT_EQUAL_INT_MESSAGE(ENIGMA_FAILURE, ret, failure);
    ;
}

void test_enigma_init_random_config(void) {
    enigma_t enigma;
    enigma_init_random_config(&enigma);

    TEST_ASSERT_TRUE_MESSAGE(enigma.rotor_count == 3 || enigma.rotor_count == 4,
                             "Expected rotor count to be set to a valid numbre");
    for (int i = 0; i < enigma.rotor_count; i++) {
        TEST_ASSERT_TRUE_MESSAGE(enigma.rotor_indices[i] >= 0
                                     && enigma.rotor_indices[i] < ENIGMA_ALPHA_SIZE,
                                 "Expected rotor index to be set to a valid number");
    }
    for (size_t i = 0; i < strlen(enigma.plugboard); i++) {
        TEST_ASSERT_TRUE_MESSAGE(enigma.plugboard[i] >= 'A' && enigma.plugboard[i] <= 'Z',
                                 "Expected plugboard characters to be valid letter");
    }
    TEST_ASSERT_NOT_NULL_MESSAGE(enigma.reflector->name, "Expected reflector to be set");
}

void test_enigma_init_random_config_WithInvalidArguments(void) {
    int ret = enigma_init_random_config(NULL);
    TEST_ASSERT_EQUAL_INT_MESSAGE(ENIGMA_FAILURE, ret, failure);
}

void test_rotate(void) {
    int idx = 0;
    rotate(&idx);
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, idx, "Expected index to be incremented");

    idx = ENIGMA_ALPHA_SIZE - 1;
    rotate(&idx);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, idx, "Expected index to wrap back to zero");
}

void test_rotate_rotors(void) {
    enigma_t       enigma;
    enigma_rotor_t rotor1 = enigma_rotor_I;
    enigma_rotor_t rotor2 = enigma_rotor_II;
    enigma_rotor_t rotor3 = enigma_rotor_III;

    enigma_init_default_config(&enigma);
    enigma.rotor_count      = 3;
    enigma.rotors[0]        = &rotor1;
    enigma.rotors[1]        = &rotor2;
    enigma.rotors[2]        = &rotor3;

    enigma.rotor_indices[0] = 0;
    rotate_rotors(&enigma);

    TEST_ASSERT_EQUAL_INT_MESSAGE(1, enigma.rotor_indices[0], "Expected first rotor to rotate");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0,
                                  enigma.rotor_indices[1],
                                  "Expected second rotor not to rotate when not on a notch");

    enigma.rotor_indices[0] = 9;
    enigma.rotor_indices[1] = 17;
    rotor2.fwd_indices[17]  = 25;
    rotor2.notches[0]       = 25;
    rotor2.notches_count    = 1;
    rotate_rotors(&enigma);

    TEST_ASSERT_EQUAL_INT_MESSAGE(10, enigma.rotor_indices[0], "Expected first rotor to rotate");
    TEST_ASSERT_EQUAL_INT_MESSAGE(18,
                                  enigma.rotor_indices[1],
                                  "Expected second rotor to rotate when on a notch");
}

void test_rotor_pass_forward(void) {
    int            idx   = 0;
    enigma_rotor_t rotor = {
        .fwd_indices = { 4,  10, 12, 5,  11, 6,  3,  16, 21, 25, 13, 19, 14,
                         22, 24, 7,  23, 20, 18, 15, 0,  8,  1,  17, 2,  9 },
    };

    TEST_ASSERT_EQUAL_INT_MESSAGE(4,
                                  rotor_pass_forward(&rotor, idx, 0),
                                  "Expected rotor to pass forward properly"); // A -> E
    TEST_ASSERT_EQUAL_INT_MESSAGE(10,
                                  rotor_pass_forward(&rotor, idx, 1),
                                  "Expected rotor to pass forward properly"); // B -> K
}

void test_rotor_pass_reverse(void) {
    int            idx   = 0;
    enigma_rotor_t rotor = {
        .rev_indices = { 20, 22, 24, 6, 0,  3,  5,  15, 21, 25, 1,  4,  2,
                         10, 12, 19, 7, 23, 18, 11, 17, 8,  16, 14, 13, 9 },
    };

    TEST_ASSERT_EQUAL_INT_MESSAGE(0,
                                  rotor_pass_reverse(&rotor, idx, 4),
                                  "Expected rotor to pass properly in reverse"); // E -> A
    TEST_ASSERT_EQUAL_INT_MESSAGE(1,
                                  rotor_pass_reverse(&rotor, idx, 10),
                                  "Expected rotor to pass properly in reverse"); // K -> B
}

void test_substitute(void) {
    enigma_t enigma;
    strcpy(enigma.plugboard, "ABCD");

    TEST_ASSERT_EQUAL_CHAR_MESSAGE('B',
                                   substitute(enigma.plugboard, 'A'),
                                   "Expected substitution to work properly");
    TEST_ASSERT_EQUAL_CHAR_MESSAGE('A',
                                   substitute(enigma.plugboard, 'B'),
                                   "Expected substitution to work properly");

    TEST_ASSERT_EQUAL_CHAR_MESSAGE('D',
                                   substitute(enigma.plugboard, 'C'),
                                   "Expected substitution to work properly");
    TEST_ASSERT_EQUAL_CHAR_MESSAGE('C',
                                   substitute(enigma.plugboard, 'D'),
                                   "Expected substitution to work properly");

    TEST_ASSERT_EQUAL_CHAR_MESSAGE('E',
                                   substitute(enigma.plugboard, 'E'),
                                   "Expected substitution to work properly");
}

void test_enigma_version(void) { TEST_ASSERT_EQUAL_STRING(LIBENIGMA_VERSION, enigma_version()); }

// --- enigma_t getter/setter tests ---

void test_enigma_get_plugboard(void) {
    enigma_t    enigma;
    const char* plugboard = "ABCD";
    strcpy(enigma.plugboard, plugboard);

    TEST_ASSERT_EQUAL_STRING(plugboard, enigma_get_plugboard(&enigma));
}

void test_enigma_get_reflector(void) {
    enigma_t enigma;
    enigma.reflector = enigma_reflectors[0];

    TEST_ASSERT_EQUAL_PTR(enigma_reflectors[0], enigma_get_reflector(&enigma));
}

void test_enigma_get_rotor(void) {
    enigma_t              enigma;
    const enigma_rotor_t* rotor = enigma_rotors[0];
    enigma.rotors[0]            = rotor;
    TEST_ASSERT_EQUAL_PTR(rotor, enigma_get_rotor(&enigma, 0));
}

void test_enigma_get_rotor_count(void) {
    enigma_t enigma;
    enigma.rotor_count = 0;
    TEST_ASSERT_EQUAL_INT(0, enigma_get_rotor_count(&enigma));
}

void test_enigma_get_rotor_flag(void) {
    enigma_t enigma;
    enigma.rotor_flag = 42;
    TEST_ASSERT_EQUAL_INT(42, enigma_get_rotor_flag(&enigma));
}

void test_enigma_get_rotor_index(void) {
    enigma_t enigma;
    enigma.rotor_indices[2] = 7;
    TEST_ASSERT_EQUAL_INT(7, enigma_get_rotor_index(&enigma, 2));
}

void test_enigma_set_plugboard(void) {
    enigma_t    enigma;
    const char* plugboard = "QWERTY";
    int         ret       = enigma_set_plugboard(&enigma, plugboard);
    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_STRING(plugboard, enigma.plugboard);
}

void test_enigma_set_reflector(void) {
    enigma_t enigma;
    int      ret = enigma_set_reflector(&enigma, 0); // assuming 0 is a valid reflector index
    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_NOT_NULL(enigma.reflector);
}

void test_enigma_set_rotor(void) {
    enigma_t enigma;
    int      ret = enigma_set_rotor(&enigma, 1, 2); // set rotor 1 to rotor index 2
    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_NOT_NULL(enigma.rotors[1]);
}

void test_enigma_set_rotor_count(void) {
    enigma_t enigma;
    int      ret = enigma_set_rotor_count(&enigma, 4);
    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_INT(4, enigma.rotor_count);
}

void test_enigma_set_rotor_flag(void) {
    enigma_t enigma;
    int      ret = enigma_set_rotor_flag(&enigma, 1);
    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_INT(1, enigma.rotor_flag);
}

void test_enigma_set_rotor_index(void) {
    enigma_t enigma;
    enigma.rotor_count = 3;
    int ret            = enigma_set_rotor_index(&enigma, 2, 15);
    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_INT(15, enigma.rotor_indices[2]);
}
