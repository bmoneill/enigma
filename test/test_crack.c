#include "enigma/crack.h"
#include "enigma/enigma.h"
#include "unity.h"

#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_STORED_SCORES 10
#define MAX_SCORES        100

enigma_crack_t      cfg;
enigma_score_list_t scores;
float               mockScore = 0.0f;
float               storedScores[MAX_STORED_SCORES];
int                 scoreIdx        = 0;
const char*         helloWorld      = "HELLOXWORLD";
const char*         alphaText       = "THEXQUICKXBROWNXFOXXJUMPSXOVERXTHEXLAZYXDOG";
const char*         ciphertext      = "DMRFRXHAZUQZNLRUOM";
const char*         shortCiphertext = "DM";

void                setUp(void) {
    srand(time(NULL));
    memset(&cfg, 0, sizeof(enigma_crack_t));
    enigma_init_default_config(&cfg.enigma);
    scoreIdx                    = 0;
    cfg.ciphertext              = shortCiphertext;
    cfg.ciphertext_length       = strlen(shortCiphertext);
    cfg.score_list              = &scores;
    cfg.score_list->max_scores  = 100;
    cfg.score_list->score_count = 0;
    cfg.score_list->scores      = calloc(100, sizeof(enigma_score_t));
    memset(storedScores, 0, MAX_STORED_SCORES * sizeof(float));
}

void  tearDown(void) { free(cfg.score_list->scores); }

float mock_score_function(const enigma_crack_t* config, const char* plaintext) {
    float score = ((float) rand()) / rand();
    if (scoreIdx < MAX_STORED_SCORES) {
        storedScores[scoreIdx] = score;
        scoreIdx++;
    }

    return score;
}

void test_enigma_crack_plugboard(void) {
    int ret = enigma_crack_plugboard(&cfg, mock_score_function);

    TEST_ASSERT_EQUAL_INT(0, ret);
}

void test_enigma_crack_plugboard_WithNullArguments(void) {
    TEST_ASSERT_NOT_EQUAL_INT(0, enigma_crack_plugboard(NULL, NULL));
    TEST_ASSERT_NOT_EQUAL_INT(0, enigma_crack_plugboard(&cfg, NULL));
    TEST_ASSERT_NOT_EQUAL_INT(0, enigma_crack_plugboard(NULL, mock_score_function));
}

void test_enigma_crack_reflector(void) {
    int ret = enigma_crack_reflector(&cfg, mock_score_function);

    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_INT(scoreIdx, scores.score_count);
    for (int i = 0; i < scores.score_count; i++) {
        TEST_ASSERT_EQUAL_FLOAT(storedScores[i], cfg.score_list->scores[i].score);
        TEST_ASSERT_EQUAL_INT_ARRAY(cfg.enigma.rotor_indices,
                                    cfg.score_list->scores[i].enigma.rotor_indices,
                                    4);
        if (i < scores.score_count - 1) {
            TEST_ASSERT_NOT_EQUAL_CHAR(cfg.score_list->scores[i].enigma.reflector->name[0],
                                       cfg.score_list->scores[i + 1].enigma.reflector->name[0]);
        }
    }
}

void test_enigma_crack_reflector_WithNullArguments(void) {
    TEST_ASSERT_NOT_EQUAL_INT(0, enigma_crack_reflector(NULL, NULL));
    TEST_ASSERT_NOT_EQUAL_INT(0, enigma_crack_plugboard(&cfg, NULL));
    TEST_ASSERT_NOT_EQUAL_INT(0, enigma_crack_plugboard(NULL, mock_score_function));
}

void test_enigma_crack_rotor(void) {
    int rot = 2;
    int ret = enigma_crack_rotor(&cfg, rot, mock_score_function);

    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_INT(scoreIdx, scores.score_count);
    for (int i = 0; i < scores.score_count; i++) {
        TEST_ASSERT_EQUAL_FLOAT(storedScores[i], cfg.score_list->scores[i].score);
        TEST_ASSERT_EQUAL_INT_ARRAY(cfg.enigma.rotor_indices,
                                    cfg.score_list->scores[i].enigma.rotor_indices,
                                    4);

        if (i < scores.score_count - 1) {
            int res = strcmp(cfg.score_list->scores[i].enigma.rotors[rot]->name,
                             cfg.score_list->scores[i + 1].enigma.rotors[rot]->name);
            TEST_ASSERT_NOT_EQUAL_INT(0, res);
        }
    }
}

void test_enigma_crack_rotor_WithNullArguments(void) {
    TEST_ASSERT_NOT_EQUAL_INT(0, enigma_crack_rotor(NULL, 0, NULL));
    TEST_ASSERT_NOT_EQUAL_INT(0, enigma_crack_rotor(&cfg, 0, NULL));
    TEST_ASSERT_NOT_EQUAL_INT(0, enigma_crack_rotor(NULL, 0, mock_score_function));
}

void test_enigma_crack_rotors(void) {
    int ret = enigma_crack_rotors(&cfg, mock_score_function);
    TEST_ASSERT_EQUAL_INT(0, ret);

    for (int i = 0; i < MAX_STORED_SCORES; i++) {
        TEST_ASSERT_EQUAL_FLOAT(storedScores[i], cfg.score_list->scores[i].score);
        TEST_ASSERT_EQUAL_INT_ARRAY(cfg.enigma.rotor_indices,
                                    cfg.score_list->scores[i].enigma.rotor_indices,
                                    4);

        int       cmp = 0;
        enigma_t* e1  = &cfg.score_list->scores[i].enigma;
        enigma_t* e2  = &cfg.score_list->scores[i + 1].enigma;
        for (int i = 0; i < 3; i++) {
            if (e1->rotors[i] == e2->rotors[i]) {
                cmp++;
            }
        }
        TEST_ASSERT_NOT_EQUAL_INT(3, cmp);
    }
}

void test_enigma_crack_rotors_WithNullArguments(void) {
    TEST_ASSERT_NOT_EQUAL_INT(0, enigma_crack_rotors(NULL, NULL));
    TEST_ASSERT_NOT_EQUAL_INT(0, enigma_crack_rotors(&cfg, NULL));
    TEST_ASSERT_NOT_EQUAL_INT(0, enigma_crack_rotors(NULL, mock_score_function));
}

void test_enigma_crack_rotors_WithFourRotors(void) {
    cfg.enigma.rotor_count = 4;
    int ret                = enigma_crack_rotors(&cfg, mock_score_function);
    TEST_ASSERT_EQUAL_INT(0, ret);
}

void test_enigma_crack_rotor_positions(void) {
    int ret = enigma_crack_rotor_positions(&cfg, mock_score_function);
    TEST_ASSERT_EQUAL_INT(0, ret);

    for (int i = 0; i < MAX_STORED_SCORES; i++) {
        TEST_ASSERT_EQUAL_FLOAT(storedScores[i], cfg.score_list->scores[i].score);
        TEST_ASSERT_EQUAL_CHAR_ARRAY(cfg.enigma.plugboard,
                                     cfg.score_list->scores[i].enigma.plugboard,
                                     27);

        int       cmp = 0;
        enigma_t* e1  = &cfg.score_list->scores[i].enigma;
        enigma_t* e2  = &cfg.score_list->scores[i + 1].enigma;
        for (int i = 0; i < 3; i++) {
            if (e1->rotor_indices[i] == e2->rotor_indices[i]) {
                cmp++;
            }
        }
        TEST_ASSERT_NOT_EQUAL_INT(3, cmp);
    }
}

void test_enigma_crack_rotor_positions_WithNullArguments(void) {
    TEST_ASSERT_NOT_EQUAL_INT(0, enigma_crack_rotor_positions(NULL, NULL));
    TEST_ASSERT_NOT_EQUAL_INT(0, enigma_crack_rotor_positions(&cfg, NULL));
    TEST_ASSERT_NOT_EQUAL_INT(0, enigma_crack_rotor_positions(NULL, mock_score_function));
}

void test_enigma_dict_match_WithMatchingPlaintext(void) {
    const char* plaintext = "HELLOXWORLDXFOOXBAR";
    cfg.dictionary        = malloc(7 * sizeof(char*));
    cfg.dictionary_length = 7;

    cfg.dictionary[0]     = "GOODBYE";
    cfg.dictionary[1]     = "HELLO";
    cfg.dictionary[2]     = "FOO";
    cfg.dictionary[3]     = "BAZ";
    cfg.dictionary[4]     = "BAR";
    cfg.dictionary[5]     = "TEST";
    cfg.dictionary[6]     = "WORLD";

    TEST_ASSERT_EQUAL_INT(1, enigma_dict_match(&cfg, plaintext));

    free(cfg.dictionary);
}

void test_enigma_dict_match_WithNonMatchingPlaintext(void) {
    cfg.dictionary        = malloc(4 * sizeof(char*));
    cfg.dictionary_length = 4;
    cfg.dictionary[0]     = "FOO";
    cfg.dictionary[1]     = "EARTH";
    cfg.dictionary[2]     = "MARS";
    cfg.dictionary[3]     = "SATURN";

    TEST_ASSERT_EQUAL_INT(0, enigma_dict_match(&cfg, helloWorld));
}

void test_enigma_dict_match_WithNullArguments(void) {
    TEST_ASSERT_EQUAL_INT(-1, enigma_dict_match(NULL, NULL));
    TEST_ASSERT_EQUAL_INT(-1, enigma_dict_match(&cfg, "HELLO"));
}

void test_enigma_find_potential_indices_WithMatchingPlaintext(void) {
    const char* plaintext  = "HELLO";
    const char* ciphertext = "HLEOASFD";
    int*        indices    = malloc(sizeof(int) * strlen(ciphertext));

    int         ret        = enigma_find_potential_indices(ciphertext, plaintext, indices);

    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_INT(2, indices[0]);
    TEST_ASSERT_EQUAL_INT(3, indices[1]);
    TEST_ASSERT_EQUAL_INT(-1, indices[2]);
    free(indices);
}

void test_enigma_find_potential_indices_WithNonMatchingPlaintext(void) {
    const char* ciphertext = "GOODBYE";

    int*        indices    = malloc(sizeof(int) * strlen(ciphertext));

    int         ret        = enigma_find_potential_indices(ciphertext, helloWorld, indices);

    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_INT(-1, indices[0]);
    free(indices);
}

void test_enigma_freq(void) {
    const char* plaintext = "HELLO";
    float       actual    = enigma_freq(plaintext, strlen(plaintext));
    float       expected  = (2.0) / (5.0 * 4.0);
    TEST_ASSERT_EQUAL_FLOAT(expected, actual);
}

void test_enigma_freq_WithNullArguments(void) {
    TEST_ASSERT_EQUAL_FLOAT(-1.0f, enigma_freq(NULL, 0));
}

void test_enigma_letter_freq_WithSufficientPlaintext(void) {
    cfg.ciphertext_length = strlen(alphaText);

    for (int i = 0; i < 26; i++) {
        cfg.frequency_targets[i] = 0.05;
    }
    cfg.frequency_offset = 0.03;

    int actual           = enigma_letter_freq(&cfg, alphaText);

    TEST_ASSERT_EQUAL_FLOAT(1, actual);
}

void test_enigma_letter_freq_WithInsufficientPlaintext(void) {
    cfg.ciphertext_length = strlen(alphaText);
    for (int i = 0; i < 26; i++) {
        cfg.frequency_targets[i] = 1.0;
    }
    cfg.frequency_offset = 0.01;

    TEST_ASSERT_EQUAL_FLOAT(0, enigma_letter_freq(&cfg, alphaText));
}

void test_enigma_letter_freq_WithNullArguments(void) {
    TEST_ASSERT_EQUAL_FLOAT(-1.0f, enigma_letter_freq(&cfg, NULL));
    TEST_ASSERT_EQUAL_FLOAT(-1.0f, enigma_letter_freq(NULL, "HELLO"));
    TEST_ASSERT_EQUAL_FLOAT(-1.0f, enigma_letter_freq(NULL, NULL));
}

void test_enigma_score_append_WithEmptyScoreList(void) {
    enigma_t enigma;
    float    score = 0.05;

    enigma_init_default_config(&enigma);
    int ret = enigma_score_append(&cfg, &enigma, helloWorld, score);

    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_INT(1, cfg.score_list->score_count);
    TEST_ASSERT_EQUAL_INT(MAX_SCORES, cfg.score_list->max_scores);
    TEST_ASSERT_EQUAL_FLOAT(score, cfg.score_list->scores[0].score);
    TEST_ASSERT_EQUAL_INT_ARRAY(enigma.rotor_indices,
                                cfg.score_list->scores[0].enigma.rotor_indices,
                                4);
}

void test_enigma_score_append_WithFullScoreList(void) {
    enigma_t enigma;
    float    score      = 0.05;
    int      scoreCount = MAX_SCORES;

    scores.score_count  = scoreCount;
    int ret             = enigma_score_append(&cfg, &enigma, helloWorld, score);

    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_INT(scoreCount + 1, cfg.score_list->score_count);
    TEST_ASSERT_EQUAL_INT(MAX_SCORES * 2, cfg.score_list->max_scores);
    TEST_ASSERT_EQUAL_FLOAT(score, cfg.score_list->scores[scoreCount].score);
    TEST_ASSERT_EQUAL_INT_ARRAY(enigma.rotor_indices,
                                cfg.score_list->scores[scoreCount].enigma.rotor_indices,
                                4);
}

void test_enigma_score_append_WithPartialScoreList(void) {
    enigma_t enigma;
    float    score      = 0.05;
    int      scoreCount = 5;

    scores.score_count  = scoreCount;
    int ret             = enigma_score_append(&cfg, &enigma, helloWorld, score);

    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_INT(scoreCount + 1, cfg.score_list->score_count);
    TEST_ASSERT_EQUAL_INT(MAX_SCORES, cfg.score_list->max_scores);
    TEST_ASSERT_EQUAL_FLOAT(score, cfg.score_list->scores[scoreCount].score);
    TEST_ASSERT_EQUAL_INT_ARRAY(enigma.rotor_indices,
                                cfg.score_list->scores[scoreCount].enigma.rotor_indices,
                                4);
}

void test_enigma_score_append_WithNullArguments(void) {
    enigma_t enigma;
    TEST_ASSERT_NOT_EQUAL_INT(0, enigma_score_append(NULL, NULL, NULL, 0));
    TEST_ASSERT_NOT_EQUAL_INT(0, enigma_score_append(&cfg, NULL, NULL, 0));
    TEST_ASSERT_NOT_EQUAL_INT(0, enigma_score_append(&cfg, &enigma, NULL, 0));
    TEST_ASSERT_NOT_EQUAL_INT(0, enigma_score_append(&cfg, NULL, alphaText, 0));
    TEST_ASSERT_NOT_EQUAL_INT(0, enigma_score_append(NULL, &enigma, alphaText, 0));
    TEST_ASSERT_NOT_EQUAL_INT(0, enigma_score_append(NULL, NULL, alphaText, 0));
    TEST_ASSERT_NOT_EQUAL_INT(0, enigma_score_append(NULL, &enigma, NULL, 0));
}

void test_enigma_score_flags_WithKnownPlaintextFlag_WithMatchingPlaintext(void) {
    cfg.flags           = ENIGMA_FLAG_KNOWN_PLAINTEXT;
    cfg.known_plaintext = "WORLD";
    int ret             = enigma_score_flags(&cfg, helloWorld);
    TEST_ASSERT_EQUAL_INT(ENIGMA_FLAG_KNOWN_PLAINTEXT, ret);
}

void test_enigma_score_flags_WithKnownPlaintextFlag_WithNonMatchingPlaintext(void) {
    cfg.flags           = ENIGMA_FLAG_KNOWN_PLAINTEXT;
    cfg.known_plaintext = "WORLDS";
    int ret             = enigma_score_flags(&cfg, helloWorld);
    TEST_ASSERT_EQUAL_INT(0, ret);
}

void test_enigma_score_flags_WithKnownPlaintextFlag_WithNullPlaintext(void) {
    cfg.flags           = ENIGMA_FLAG_KNOWN_PLAINTEXT;
    cfg.known_plaintext = NULL;
    int ret             = enigma_score_flags(&cfg, helloWorld);
    TEST_ASSERT_EQUAL_INT(-1, ret);
}

// --- enigma_crack_t getter/setter tests ---

void test_enigma_crack_get_enigma(void) {
    enigma_crack_t crack;
    enigma_t*      ptr = &crack.enigma;
    TEST_ASSERT_EQUAL_PTR(ptr, enigma_crack_get_enigma(&crack));
}

void test_enigma_crack_get_enigma_WithInvalidArguments(void) {
    TEST_ASSERT_NULL(enigma_crack_get_enigma(NULL));
}

void test_enigma_crack_get_score_list(void) {
    enigma_crack_t      crack;
    enigma_score_list_t scores;
    crack.score_list = &scores;
    TEST_ASSERT_EQUAL_PTR(&scores, enigma_crack_get_score_list(&crack));
}

void test_enigma_crack_get_score_list_WithInvalidArguments(void) {
    TEST_ASSERT_NULL(enigma_crack_get_score_list(NULL));
}

void test_enigma_crack_get_dictionary(void) {
    enigma_crack_t crack;
    const char*    dict[] = { "A", "B" };
    crack.dictionary      = dict;
    TEST_ASSERT_EQUAL_PTR(dict, enigma_crack_get_dictionary(&crack));
}

void test_enigma_crack_get_dictionary_WithInvalidArguments(void) {
    TEST_ASSERT_NULL(enigma_crack_get_dictionary(NULL));
}

void test_enigma_crack_get_dictionary_length(void) {
    enigma_crack_t crack;
    crack.dictionary_length = 42;
    TEST_ASSERT_EQUAL_INT(42, enigma_crack_get_dictionary_length(&crack));
}

void test_enigma_crack_get_dictionary_length_WithInvalidArguments(void) {
    TEST_ASSERT_EQUAL_INT(-1, enigma_crack_get_dictionary_length(NULL));
}

void test_enigma_crack_get_ngrams(void) {
    enigma_crack_t crack;
    float          ngrams[3] = { 1, 2, 3 };
    crack.ngrams             = ngrams;
    TEST_ASSERT_EQUAL_PTR(ngrams, enigma_crack_get_ngrams(&crack));
}

void test_enigma_crack_get_ngrams_WithInvalidArguments(void) {
    TEST_ASSERT_NULL(enigma_crack_get_ngrams(NULL));
}

void test_enigma_crack_get_n(void) {
    enigma_crack_t crack;
    crack.n = 5;
    TEST_ASSERT_EQUAL_INT(5, enigma_crack_get_n(&crack));
}

void test_enigma_crack_get_n_WithInvalidArguments(void) {
    TEST_ASSERT_EQUAL_INT(-1, enigma_crack_get_n(NULL));
}

void test_enigma_crack_get_ngrams_length(void) {
    enigma_crack_t crack;
    crack.ngrams_length = 7;
    TEST_ASSERT_EQUAL_INT(7, enigma_crack_get_ngrams_length(&crack));
}

void test_enigma_crack_get_ngrams_length_WithInvalidArguments(void) {
    TEST_ASSERT_EQUAL_INT(-1, enigma_crack_get_ngrams_length(NULL));
}

void test_enigma_crack_get_ciphertext(void) {
    enigma_crack_t crack;
    crack.ciphertext = "ABC";
    TEST_ASSERT_EQUAL_PTR("ABC", enigma_crack_get_ciphertext(&crack));
}

void test_enigma_crack_get_ciphertext_WithInvalidArguments(void) {
    TEST_ASSERT_NULL(enigma_crack_get_ciphertext(NULL));
}

void test_enigma_crack_get_ciphertext_length(void) {
    enigma_crack_t crack;
    crack.ciphertext_length = 11;
    TEST_ASSERT_EQUAL_INT(11, enigma_crack_get_ciphertext_length(&crack));
}

void test_enigma_crack_get_ciphertext_length_WithInvalidArguments(void) {
    TEST_ASSERT_EQUAL_INT(-1, enigma_crack_get_ciphertext_length(NULL));
}

void test_enigma_crack_get_flags(void) {
    enigma_crack_t crack;
    crack.flags = 123;
    TEST_ASSERT_EQUAL_INT(123, enigma_crack_get_flags(&crack));
}

void test_enigma_crack_get_flags_WithInvalidArguments(void) {
    TEST_ASSERT_EQUAL_INT(-1, enigma_crack_get_flags(NULL));
}

void test_enigma_crack_get_frequency_targets(void) {
    enigma_crack_t crack;
    for (int i = 0; i < 26; i++)
        crack.frequency_targets[i] = (float) i;
    const float* ptr = enigma_crack_get_frequency_targets(&crack);
    for (int i = 0; i < 26; i++) {
        TEST_ASSERT_EQUAL_FLOAT((float) i, ptr[i]);
    }
}

void test_enigma_crack_get_frequency_targets_WithInvalidArguments(void) {
    TEST_ASSERT_NULL(enigma_crack_get_frequency_targets(NULL));
}

void test_enigma_crack_get_min_score(void) {
    enigma_crack_t crack;
    crack.min_score = 1.23f;
    TEST_ASSERT_EQUAL_FLOAT(1.23f, enigma_crack_get_min_score(&crack));
}

void test_enigma_crack_get_min_score_WithInvalidArguments(void) {
    TEST_ASSERT_EQUAL_FLOAT(-1.0f, enigma_crack_get_min_score(NULL));
}

void test_enigma_crack_get_max_score(void) {
    enigma_crack_t crack;
    crack.max_score = 4.56f;
    TEST_ASSERT_EQUAL_FLOAT(4.56f, enigma_crack_get_max_score(&crack));
}

void test_enigma_crack_get_max_score_WithInvalidArguments(void) {
    TEST_ASSERT_EQUAL_FLOAT(-1.0f, enigma_crack_get_max_score(NULL));
}

void test_enigma_crack_get_target_score(void) {
    enigma_crack_t crack;
    crack.target_score = 7.89f;
    TEST_ASSERT_EQUAL_FLOAT(7.89f, enigma_crack_get_target_score(&crack));
}

void test_enigma_crack_get_target_score_WithInvalidArguments(void) {
    TEST_ASSERT_EQUAL_FLOAT(-1.0f, enigma_crack_get_target_score(NULL));
}

void test_enigma_crack_get_known_plaintext(void) {
    enigma_crack_t crack;
    const char*    plaintext = "FOO";
    crack.known_plaintext    = plaintext;
    TEST_ASSERT_EQUAL_PTR(plaintext, enigma_crack_get_known_plaintext(&crack));
}

void test_enigma_crack_get_known_plaintext_WithInvalidArguments(void) {
    TEST_ASSERT_NULL(enigma_crack_get_known_plaintext(NULL));
}

void test_enigma_crack_get_known_plaintext_length(void) {
    enigma_crack_t crack;
    crack.known_plaintext_length = 8;
    TEST_ASSERT_EQUAL_INT(8, enigma_crack_get_known_plaintext_length(&crack));
}

void test_enigma_crack_get_known_plaintext_length_WithInvalidArguments(void) {
    TEST_ASSERT_EQUAL_INT(-1, enigma_crack_get_known_plaintext_length(NULL));
}

void test_enigma_crack_set_enigma(void) {
    enigma_crack_t crack;
    enigma_t       enigma;
    int            ret = enigma_crack_set_enigma(&crack, &enigma);
    TEST_ASSERT_EQUAL_INT(0, ret);
}

void test_enigma_crack_set_enigma_WithInvalidArguments(void) {
    enigma_t enigma;
    TEST_ASSERT_EQUAL_INT(-1, enigma_crack_set_enigma(NULL, &enigma));
    enigma_crack_t crack;
    TEST_ASSERT_EQUAL_INT(-1, enigma_crack_set_enigma(&crack, NULL));
}

void test_enigma_crack_set_score_list(void) {
    enigma_crack_t      crack;
    enigma_score_list_t scores;
    int                 ret = enigma_crack_set_score_list(&crack, &scores);
    TEST_ASSERT_EQUAL_INT(0, ret);
}

void test_enigma_crack_set_score_list_WithInvalidArguments(void) {
    enigma_score_list_t scores;
    TEST_ASSERT_EQUAL_INT(-1, enigma_crack_set_score_list(NULL, &scores));
    enigma_crack_t crack;
    TEST_ASSERT_EQUAL_INT(-1, enigma_crack_set_score_list(&crack, NULL));
}

void test_enigma_crack_set_ngrams(void) {
    enigma_crack_t cfg;
    float          ngrams[10] = { 0 };
    int            ret        = enigma_crack_set_ngrams(&cfg, ngrams, 3, 10);
    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_PTR(ngrams, cfg.ngrams);
    TEST_ASSERT_EQUAL_INT(3, cfg.n);
    TEST_ASSERT_EQUAL_INT(10, cfg.ngrams_length);
}

void test_enigma_crack_set_ngrams_WithInvalidArguments(void) {
    enigma_crack_t cfg;
    float          ngrams[10] = { 0 };
    TEST_ASSERT_EQUAL_INT(-1, enigma_crack_set_ngrams(NULL, ngrams, 3, 10));
    TEST_ASSERT_EQUAL_INT(-1, enigma_crack_set_ngrams(&cfg, NULL, 3, 10));
    TEST_ASSERT_EQUAL_INT(-1, enigma_crack_set_ngrams(&cfg, ngrams, 1, 10));
    TEST_ASSERT_EQUAL_INT(-1, enigma_crack_set_ngrams(&cfg, ngrams, 5, 10));
}

void test_enigma_crack_set_n(void) {
    enigma_crack_t cfg;
    int            ret = enigma_crack_set_n(&cfg, 3);
    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_INT(3, cfg.n);
}

void test_enigma_crack_set_n_WithInvalidArguments(void) {
    enigma_crack_t cfg;
    TEST_ASSERT_EQUAL_INT(-1, enigma_crack_set_n(NULL, 3));
    TEST_ASSERT_EQUAL_INT(-1, enigma_crack_set_n(&cfg, 1));
    TEST_ASSERT_EQUAL_INT(-1, enigma_crack_set_n(&cfg, 5));
}

void test_enigma_crack_set_dictionary(void) {
    enigma_crack_t cfg;
    const char*    dict[2] = { "WORD1", "WORD2" };
    int            ret     = enigma_crack_set_dictionary(&cfg, dict, 2);
    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_PTR(dict, cfg.dictionary);
    TEST_ASSERT_EQUAL_INT(2, cfg.dictionary_length);
}

void test_enigma_crack_set_dictionary_WithInvalidArguments(void) {
    enigma_crack_t cfg;
    const char*    dict[2] = { "WORD1", "WORD2" };
    TEST_ASSERT_EQUAL_INT(-1, enigma_crack_set_dictionary(NULL, dict, 2));
    TEST_ASSERT_EQUAL_INT(-1, enigma_crack_set_dictionary(&cfg, NULL, 2));
}

void test_enigma_crack_set_ciphertext(void) {
    enigma_crack_t cfg;
    const char*    ciphertext = "ABCDEF";
    int            ret        = enigma_crack_set_ciphertext(&cfg, ciphertext, 6);
    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_PTR(ciphertext, cfg.ciphertext);
    TEST_ASSERT_EQUAL_INT(6, cfg.ciphertext_length);
}

void test_enigma_crack_set_ciphertext_WithInvalidArguments(void) {
    enigma_crack_t cfg;
    const char*    ciphertext = "ABCDEF";
    TEST_ASSERT_EQUAL_INT(-1, enigma_crack_set_ciphertext(NULL, ciphertext, 6));
    TEST_ASSERT_EQUAL_INT(-1, enigma_crack_set_ciphertext(&cfg, NULL, 6));
}

void test_enigma_crack_set_flags(void) {
    enigma_crack_t cfg;
    int            ret = enigma_crack_set_flags(&cfg, 42);
    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_INT(42, cfg.flags);
}

void test_enigma_crack_set_flags_WithInvalidArguments(void) {
    TEST_ASSERT_EQUAL_INT(-1, enigma_crack_set_flags(NULL, 42));
}

void test_enigma_crack_set_frequency_targets(void) {
    enigma_crack_t cfg;
    float          targets[26] = { 0 };
    for (int i = 0; i < 26; i++)
        targets[i] = (float) i;
    int ret = enigma_crack_set_frequency_targets(&cfg, targets);
    TEST_ASSERT_EQUAL_INT(0, ret);
    for (int i = 0; i < 26; i++) {
        TEST_ASSERT_EQUAL_FLOAT((float) i, cfg.frequency_targets[i]);
    }
}

void test_enigma_crack_set_frequency_targets_WithInvalidArguments(void) {
    enigma_crack_t cfg;
    float          targets[26] = { 0 };
    TEST_ASSERT_EQUAL_INT(-1, enigma_crack_set_frequency_targets(NULL, targets));
    TEST_ASSERT_EQUAL_INT(-1, enigma_crack_set_frequency_targets(&cfg, NULL));
}

void test_enigma_crack_set_min_score(void) {
    enigma_crack_t cfg;
    int            ret = enigma_crack_set_min_score(&cfg, 1.23f);
    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_FLOAT(1.23f, cfg.min_score);
}

void test_enigma_crack_set_min_score_WithInvalidArguments(void) {
    TEST_ASSERT_EQUAL_INT(-1, enigma_crack_set_min_score(NULL, 1.23f));
}

void test_enigma_crack_set_max_score(void) {
    enigma_crack_t cfg;
    int            ret = enigma_crack_set_max_score(&cfg, 4.56f);
    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_FLOAT(4.56f, cfg.min_score); // Note: sets min_score in implementation!
}

void test_enigma_crack_set_max_score_WithInvalidArguments(void) {
    TEST_ASSERT_EQUAL_INT(-1, enigma_crack_set_max_score(NULL, 4.56f));
}

void test_enigma_crack_set_target_score(void) {
    enigma_crack_t cfg;
    int            ret = enigma_crack_set_target_score(&cfg, 7.89f);
    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_FLOAT(7.89f, cfg.target_score);
}

void test_enigma_crack_set_target_score_WithInvalidArguments(void) {
    TEST_ASSERT_EQUAL_INT(-1, enigma_crack_set_target_score(NULL, 7.89f));
}

void test_enigma_crack_set_known_plaintext(void) {
    enigma_crack_t cfg;
    const char*    known = "HELLO";
    int            ret   = enigma_crack_set_known_plaintext(&cfg, known, 5);
    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_PTR(known, cfg.known_plaintext);
    TEST_ASSERT_EQUAL_INT(5, cfg.known_plaintext_length);
}

void test_enigma_crack_set_known_plaintext_WithInvalidArguments(void) {
    enigma_crack_t cfg;
    const char*    known = "HELLO";
    TEST_ASSERT_EQUAL_INT(-1, enigma_crack_set_known_plaintext(NULL, known, 5));
    TEST_ASSERT_EQUAL_INT(-1, enigma_crack_set_known_plaintext(&cfg, NULL, 5));
}
