#include "enigma/crack.h"
#include "enigma/enigma.h"
#include "enigma/io.h"

#include <stdio.h>
#include <string.h>

#define USAGE                                                                                      \
    "Available commands:\n"                                                                        \
    "show config: Display the current config\n"                                                    \
    "show scores: Display top scoring plaintext candidates so far\n"                               \
    "vary <attribute>: Vary the given attribute in the next iteration"                             \
    "loads <type> <input>: Load the given data type from an inputted string"                       \
    "loadf <type> <path>: Load the given data type from a file"                                    \
    "set <attribute> <value>: Set an attribute to a given value"                                   \
    "crack: Analyze the current Enigma"                                                            \
    "exit: Quit the REPL\n\n"

#define ATTR_REFLECTOR "reflector"
#define ATTR_PLUGBOARD "plugboard"

#define ATTR_MAX_PLUGBOARD_SIZE "max_plugboard_size"
#define ATTR_MIN_PLUGBOARD_SIZE "min_plugboard_size"

#define ATTR_ROTOR_POSITIONS "rotor_positions"
#define ATTR_ROTORS          "rotors"
#define ATTR_ROTOR_POSITION  "rotor_position"
#define ATTR_ROTOR           "rotor"
#define ATTR_METHOD          "method"

#define METHOD_IOC   "ioc"
#define METHOD_NGRAM "ngram"

#define TYPE_DICT   "dict"
#define TYPE_NGRAMS "ngrams"
#define TYPE_FREQ   "freq"

EnigmaCrackParams cfg;

void              start_repl(EnigmaCrackParams* initial_cfg) {
    memcpy(&cfg, initial_cfg, sizeof(EnigmaCrackParams));

    char input[256];
    char output[256];

    while (1) {
        printf("enigmacrack> ");
        if (!fgets(input, sizeof(input), stdin)) {
            break; // EOF
        }

        input[strcspn(input, "\n")] = 0;

        if (strcmp(input, "exit") == 0) {
            break;
        } else if (strcmp(input, "show config") == 0) {
            enigma_print_config(&cfg.enigma, output, sizeof(output));
        } else if (strcmp(input, "show scores") == 0) {
            enigma_score_print(cfg.score_list);
        } else if ()
    }
    else {
        printf("Unknown command: %s\n", input);
        printf("Available commands: show config, show scores, exit\n");
    }
}
}
