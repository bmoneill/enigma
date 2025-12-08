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
  /**
   * Initialize a new Enigma struct.
   * @returns A pointer to a malloc'd Enigma
   */
  initEnigma: () => {
    const ptr = Module._enigma_new();
    return ptr;
  },

  /**
   * Print the configuration of an Enigma struct to the console.
   * @param {*} ptr A pointer to an Enigma struct
   */
  printConfig: (ptr) => {
    const bufSize = 1024;
    const bufPtr = Module._malloc(bufSize);
    Module._enigma_print_config(ptr, bufPtr);
    // Read the null-terminated string from WASM memory
    const s = Module.UTF8ToString(bufPtr);
    console.log(s);
    Module._free(bufPtr);
  },

  /**
   * Encode a string using an Enigma struct.
   * @param {*} ptr A pointer to an Enigma struct
   * @param {*} str The string to encode
   * @returns The encoded string
   */
  encodeString: (ptr, str) => {
    const bufSize = str.length + 1;
    const inputBuf = Module._malloc(bufSize);
    const outputBuf = Module._malloc(bufSize);
    Module.stringToUTF8(str, inputBuf, bufSize);
    Module._enigma_encode_string(ptr, inputBuf, outputBuf, bufSize - 1);
    const result = Module.UTF8ToString(outputBuf);
    Module._free(inputBuf);
    Module._free(outputBuf);
    return result;
  },
  setPlugboard: (ptr, plugboard) => {
    const plugboardPtr = Module._malloc(plugboard.length);
    Module.stringToUTF8(plugboard, plugboardPtr, plugboard.length + 1);
    Module._enigma_set_plugboard(ptr, plugboardPtr);
    Module._free(plugboardPtr);
  },
};
