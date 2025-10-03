# Enigma machine simulator and cryptanalysis tools

## Building

### Linux

#### Prerequisites

* cmake

```shell
cmake .
cmake --build .
```

### About the Enigma

The Enigma was a cipher device used by the Germans during World War II to encrypt military communication.
It uses a very complex wiring system to scramble the input.

When a key is pressed, the signal first passes through the plugboard, which swaps the letter with another according
to its configuration. It then travels through three rotors, each further scrambling the signal. Next, the signal
reaches the reflector, which further scrambles the signal. Then, the signal passes through the rotors again in reverse.
Finally, the signal passes through the plugboard again, and the resulting encoded letter lights up on the lampboard.
The Enigma's encryption was symmetric &mdash; ciphertext is decrypted using the same key as was used to create it.

## enigmacli

This is a CLI M3 Enigma simulator.

### Usage

```shell
$ enigmacli [-s plugboard] [-w rotors] [-p rotor_positions] [-u reflector]
```

`enigmacli` reads from stdin and outputs to stdout by default, and is case-agnostic.

The following options are supported:

* `-p`: Set the initial rotor (Walze) positions (e.g. `'ABC'`)
* `-s`: Set the plugboard (Steckerbrett) configuration (e.g. `'ABCDEF'`)
* `-u`: Set the reflector (Umkehrwalze) (e.g. `'B'`)
* `-w`: Set the rotor (Walze) configuration (e.g. `'I II III'`)

Here is an example encryption/decryption with a custom rotor/plugboard configuration:

```shell
$ enigmacli -s "ABCD" -w "I II IV" -p "XYZ"
HELLO # user input
MWQHY # output
$ enigmacli -s "ABCD" -w "I II IV" -p "XYZ" # same configuration
MWQHY
HELLO
```

## enigmacrack

This is a program to crack the Enigma using Index of Coincidence, n-grams, and brute force methods.
Essentially, we run our ciphertext through each set of rotors and see if the resulting
plaintext looks a bit more like English or whatever language the plaintext should be in,
using letter frequencies and n-grams, or groups of letters. Then, we can do the same thing
for the rotor positions, reflector, and plugboard.

This tool is not fully automated. If no settings are known, you will likely have to run
`enigmacrack` at least once for each component of an Enigma key. The output will contain the most
likely fits, but the top fit may not always be in the actual key.

### Usage

```shell
enigmacrack method [target] [options] [ciphertext]
```

The following options are supported:

#### Enigma Options

These options are for settings that are already known.

* `-w rotors`: Set the rotor (Walzen) configuration (e.g. 'I II III')
* `-p positions`: Set the initial rotor positions (e.g. 'ABC')
* `-u reflector`: Set the reflector (Umkehrwalze) (e.g. 'B')
* `-s plugboard`: Set the plugboard (Steckerbrett) configuration (e.g. 'ABCDEF')

#### Cryptanalysis options

* `-c plaintext`: Set the known plaintext
* `-C position`: Set the known plaintext index
* `-d file`: Set the dictionary file to use
* `-l language`: Set the language to use ('english' or 'german')
* `-m float`: Minimum score threshold for n-gram and IOC methods
* `-M float`: Maximum score threshold for n-gram and IOC methods
* `-n file`: n-gram bank to load
* `-S count`: Set the maximum number of plugboard settings (e.g. '10')
* `-t threadCount`: Set the number of threads to use

method must be one of the following:

* `brute`: Brute force
* `ioc`: Index of Coincidence (currently unimplemented)
* `bigram`: Bigram analysis (currently unimplemented)
* `trigram`: Trigram analysis (currently unimplemented)
* `quadgram`: Quadgram analysis (currently unimplemented)

target must be one of the following:

* `rotors`: Rotors
* `positions`: Rotor positions
* `plugboard`: Plugboard settings
* `reflector`: Reflector

If a file is not given, `enigmacrack` will read from standard input.

## Further Reading

* [The Cryptographic Mathematics of Enigma](https://www.nsa.gov/portals/75/documents/about/cryptologic-heritage/historical-figures-publications/publications/wwii/CryptoMathEnigma_Miller.pdf)
* [Enigma Cipher Machine on Crypto Museum](https://www.cryptomuseum.com/crypto/enigma/index.htm)
* [The Turing-Welchman Bombe (National Museum of Computing)](https://www.tnmoc.org/bombe)
* [Cracking Enigma in 2021 (Computerphile on YouTube)](https://youtu.be/RzWB5jL5RX0)

## License

Copyright (c) 2025 Ben O'Neill <ben@oneill.sh>. This work is released under the
terms of the MIT License. See [LICENSE](LICENSE) for the license terms.
