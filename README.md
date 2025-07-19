# Enigma machine simulator and Bombe

This is a CLI M3 Enigma simulator and Bombe.

## Building

```shell
make
sudo make install
```

To enable verbose messages for debugging, uncomment the `CFLAGS += -DVERBOSE` line in [config.mk](config.mk).

## Enigma Simulator

### About the Enigma

The Enigma was a cipher device used by the Germans during World War II to encrypt military communication.
It uses a very complex wiring system to scramble the input.

When a key is pressed, the signal first passes through the plugboard, which swaps the letter with another according
to its configuration. It then travels through three rotors, each further scrambling the signal. Next, the signal
reaches the reflector, which further scrambles the signal. Then, the signal passes through the rotors again in reverse.
Finally, the signal passes through the plugboard again, and the resulting encoded letter lights up on the lampboard.
The Enigma's encryption was symmetric &mdash; ciphertext is decrypted using the same key as was used to create it.

### Usage

```shell
$ enigma [-s plugboard] [-w rotors] [-p rotor_positions] [-u reflector]
```

`enigma` reads from stdin and outputs to stdout by default, and is case-agnostic.

The following options are supported:

* `-p`: Set the initial rotor (Walze) positions (e.g. `'ABC'`)
* `-s`: Set the plugboard (Steckerbrett) configuration (e.g. `'ABCDEF'`)
* `-u`: Set the reflector (Umkehrwalze) (e.g. `'B'`)
* `-w`: Set the rotor (Walze) configuration (e.g. `'I II III'`)

Here is an example encryption/decryption with a custom rotor/plugboard configuration:

```shell
$ enigma -s "ABCD" -w "I II IV" -p "XYZ"
HELLO # user input
MWQHY # output
$ enigma -s "ABCD" -w "I II IV" -p "XYZ" # same configuration
MWQHY
HELLO
```

## Bombe

This is a program based on the machine used to crack Enigma during World War II.
`bombe` must take at least 3 arguments: the crib, the index where the crib should be in
the ciphertext, and the ciphertext. The crib is a piece of known plaintext. For example,
during the war, it was common for the Germans to begin their messages with weather reports,
so the German for "weather report" could be used to crack the message and get the
day's Enigma configuration.

`bombe` will output all Enigma configurations containing the plaintext crib
string at the given index, and the resulting plaintext. By default, it is fairly slow,
so multithreading is supported.

### Usage

```shell
$ bombe [-i index] [-t thread_count] -c crib ciphertext
```

The following options are supported:

* `-c`: Crib string (required)
* `-i`: Crib string index in plaintext
* `-t`: Number of threads to use (default: 1)

If the index is not provided, `bombe` will find all potential indices and run on
each one. Indices are valid if each character in the crib string does not match
the corresponding ciphertext character (a letter may not be encoded into itself).

### Example

```shell
$ ./bombe -i 0 -c "HELLO" -t 32 "ILBDAAMTAZ"
Rotors: II (F)  VI (Y), V (E) | Reflector: B | Plaintext: HELLONLLON
Rotors: III (A)  II (A), I (A) | Reflector: B | Plaintext: HELLOWORLD # actual plaintext
Rotors: III (A)  I (C), V (E) | Reflector: C | Plaintext: HELLOJERSH
Rotors: III (A)  II (K), I (B) | Reflector: B | Plaintext: HELLOSVRTA
Rotors: III (A)  IV (Y), V (K) | Reflector: C | Plaintext: HELLOJJRZH
Rotors: III (A)  VI (V), VIII (Q) | Reflector: C | Plaintext: HELLOSHRTF
Rotors: III (A)  VI (U), V (X) | Reflector: C | Plaintext: HELLOZHRMH
Rotors: III (A)  VI (B), V (Z) | Reflector: C | Plaintext: HELLOOBRXU
Rotors: III (A)  VIII (P), VII (T) | Reflector: A | Plaintext: HELLOXCRUE
```

### Performance/Multithreading

Multithreading reduces the runtime by a significant margin. Since we are
essentially looping through all the possible Enigma configurations, this is a
task that is easily parallelizable.

Inlining most of the arithmetic functions and using indices for the rotor/reflector
alphabets cuts down the time significantly. Initially, on one thread, it took
around 20 seconds to run.

Below are single- and multi-threaded times on my 16-core i9-10885H.

```shell
$ time ./bombe -i 0 -c "HELLO" -t 1 "ILBDAAMTAZ" # 1 thread
real    0m3.349s
user    0m3.257s
sys     0m0.051s
$ time ./bombe -i 0 -c "HELLO" -t 64 "ILBDAAMTAZ" # 64 threads
real    0m0.383s
user    0m4.986s
sys     0m0.071s
$ time ./bombe -i 0 -c "HELLO" -t 256 "ILBDAAMTAZ" # 256 threads
real    0m0.348s
user    0m4.995s
sys     0m0.067s
```

## Planned Features

* Bombe should use a dictionary to highlight found plaintext containing more actual words.

## Bugs

* No support for double-stepping rotors (yet).
* Multiple notches (such as on Rotors VI-VIII) do not work properly yet.
* Bombe doesn't support a populated plugboard.
* Probably others.

If you find a bug not listed here, submit an issue, PR, or email me with a
description and/or patch.

## Further Reading

* [The Cryptographic Mathematics of Enigma](https://www.nsa.gov/portals/75/documents/about/cryptologic-heritage/historical-figures-publications/publications/wwii/CryptoMathEnigma_Miller.pdf)
* [Enigma Cipher Machine on Crypto Museum](https://www.cryptomuseum.com/crypto/enigma/index.htm)
* [The Turing-Welchman Bombe (National Museum of Computing)](https://www.tnmoc.org/bombe)

## License

Copyright (c) 2025 Ben O'Neill <ben@oneill.sh>. This work is released under the
terms of the MIT License. See [LICENSE](LICENSE) for the license terms.
