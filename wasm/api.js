/* Struct sizes */
const ENIGMA_T_SIZE = 96;
const ENIGMA_SCORE_LIST_T_SIZE = 16;
const ENIGMA_CRACK_CONFIG_T_SIZE = 288;

/* from common.h */
const ENIGMA_ALPHA_SIZE = 26;
const ENIGMA_MAX_ROTOR_COUNT = 4;
const ENIGMA_MAX_PLUGBOARD_SETTINGS = 10;
const ENIGMA_DEFAULT_NGRAM_COUNT = 26 * 26 * 26 * 26;
const ENIGMA_LANG_ENGLISH = 0;
const ENIGMA_LANG_GERMAN = 1;

/* from crack.h */
const ENIGMA_FLAG_DICTIONARY_MATCH = 1;
const ENIGMA_FLAG_FREQUENCY = 2;
const ENIGMA_FLAG_KNOWN_PLAINTEXT = 4;

/* from ioc.h */
const ENIGMA_IOC_ENGLISH = 1.73 / 26;
const ENIGMA_IOC_ENGLISH_MIN = ENIGMA_IOC_ENGLISH - 0.25;
const ENIGMA_IOC_ENGLISH_MAX = ENIGMA_IOC_ENGLISH + 0.25;
const ENIGMA_IOC_GERMAN = 2.05 / 26;
const ENIGMA_IOC_GERMAN_MIN = ENIGMA_IOC_GERMAN - 0.25;
const ENIGMA_IOC_GERMAN_MAX = ENIGMA_IOC_GERMAN + 0.25;

const enigmaAPI = {
  initEnigma: () => {
    const ptr = Module._malloc(ENIGMA_STRUCT_SIZE);
    Module._enigma_init_default_config(ptr);
    return ptr;
  },
  printConfig: (ptr) => {
    const bufSize = 1024;
    const bufPtr = Module._malloc(bufSize);
    Module._enigma_print_config(ptr, bufPtr);
    // Read the null-terminated string from WASM memory
    const s = Module.UTF8ToString(bufPtr);
    console.log(s);
    Module._free(bufPtr);
  },
  encodeString: (ptr, str) => {
    const bufSize = str.length + 1;
    const bufPtr = Module._malloc(bufSize);
    Module.stringToUTF8(str, bufPtr, bufSize);
    Module._enigma_encode_string(ptr, bufPtr);
    const result = Module.UTF8ToString(bufPtr);
    Module._free(bufPtr);
    return result;
  },
  setRotor: (ptr, rotorIndex, rotorNum) => {
    // Set the rotor at rotorIndex to rotorNum in the enigma_t struct in WASM memory.
    // Each rotor pointer is 4 bytes (assuming 32-bit pointers in WASM).
    const rotorPtr = Module.enigma_rotors[rotorNum];
    // Write the pointer value into WASM memory at the correct offset
    Module.HEAP32[(ptr >> 2) + rotorIndex] = rotorPtr;
  },
};
