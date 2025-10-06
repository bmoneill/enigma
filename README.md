# Enigma machine simulator and cryptanalysis tools

This project aims to create a full-featured C99 library and toolset for simulating and cracking the Enigma.

## About the Enigma

The Enigma was a cipher device used by the Germans during World War II to encrypt military communication.
It uses a very complex wiring system to scramble the input.

When a key is pressed, the signal first passes through the plugboard, which swaps the letter with another according
to its configuration. It then travels through three rotors, each further scrambling the signal. Next, the signal
reaches the reflector, which further scrambles the signal. Then, the signal passes through the rotors again in reverse.
Finally, the signal passes through the plugboard again, and the resulting encoded letter lights up on the lampboard.
The Enigma's encryption was symmetric &mdash; ciphertext is decrypted using the same key as was used to create it.

## Building

### Linux

#### Prerequisites

* cmake

```shell
cmake .
cmake --build .
```

## Documentation

## enigmacli

See [doc/enigmacli.md] for information about the Enigma simulator.

## enigmacrack

See [doc/enigmacrack.md] for information about the Enigma cracking tools.


## Further Reading

* [The Cryptographic Mathematics of Enigma](https://www.nsa.gov/portals/75/documents/about/cryptologic-heritage/historical-figures-publications/publications/wwii/CryptoMathEnigma_Miller.pdf)
* [Enigma Cipher Machine on Crypto Museum](https://www.cryptomuseum.com/crypto/enigma/index.htm)
* [The Turing-Welchman Bombe (National Museum of Computing)](https://www.tnmoc.org/bombe)
* [Cracking Enigma in 2021 (Computerphile on YouTube)](https://youtu.be/RzWB5jL5RX0)

## License

Copyright (c) 2025 Ben O'Neill <ben@oneill.sh>. This work is released under the
terms of the MIT License. See [LICENSE](LICENSE) for the license terms.
