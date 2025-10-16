#include "hybrid.h"

#include "ioc.h"
#include "ngram.h"

void enigma_crack_rotor_hybrid(const enigma_crack_config_t* cfg, int rotorIndex,
                               float (*scoreFunc)(const char*, const enigma_crack_config_t*, const float*)) {
    enigma_t originalEnigma = cfg->enigma;
    enigma_t testEnigma = cfg->enigma;

    const char* rotors[] = {"I", "II", "III", "IV", "V", "VI", "VII", "VIII"};
    int numRotors = sizeof(rotors) / sizeof(rotors[0]);

    for (int i = 0; i < numRotors; i++) {
        if (rotors[i][0] == originalEnigma.rotors[0].name[0] ||
            rotors[i][0] == originalEnigma.rotors[1].name[0] ||
            rotors[i][0] == originalEnigma.rotors[2].name[0]) {
            continue; // Skip if rotor is already in use
        }

        // Set the rotor
        switch (rotorIndex) {
            case 0: testEnigma.rotors[0] = enigma_get_rotor_by_name(rotors[i]); break;
            case 1: testEnigma.rotors[1] = enigma_get_rotor_by_name(rotors[i]); break;
            case 2: testEnigma.rotors[2] = enigma_get_rotor_by_name(rotors[i]); break;
            default: return; // Invalid rotor index
        }

        // Test all starting positions
        for (char pos1 = 'A'; pos1 <= 'Z'; pos1++) {
            for (char pos2 = 'A'; pos2 <= 'Z'; pos2++) {
                for (char pos3 = 'A'; pos3 <= 'Z'; pos3++) {
                    testEnigma.rotors[0].idx = pos1;
                    testEnigma.rotors[1].idx = pos2;
                    testEnigma.rotors[2].idx = pos3;

                    char* decrypted = enigma_decrypt(&testEnigma, cfg->ciphertext);
                    float score = scoreFunc(decrypted, cfg, cfg->letterFreqTargets);
                    free(decrypted);

                    if (score >= cfg->minScore && score <= cfg->maxScore) {