#!/bin/sh

emcc -O3 -s WASM=1 -s EXPORTED_RUNTIME_METHODS='["cwrap"]' -s EXPORTED_FUNCTIONS='["_malloc", "_free", "stringToUTF8", "UTF8ToString"]' -s ALLOW_MEMORY_GROWTH=1 -I src/enigma/ src/enigma/*.c
mv a.out.js a.out.wasm webdemo/
