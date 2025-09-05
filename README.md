# Enigma machine simulator and Bombe

This is a CLI M3 Enigma simulator and Bombe.

## Building

### Linux

#### Prerequisites

* cmake

```shell
cmake .
cmake --build .
```

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

## Bombe

This is a program based on the machine used to crack Enigma during World War II.
`bombe` must take at least 3 arguments: the crib, the index where the crib should be in
the ciphertext, and the ciphertext. The crib is a piece of known plaintext. For example,
during the war, it was common for the Germans to begin their messages with weather reports,
so the German for "weather report" could be used to crack the message and get the
day's Enigma configuration.

`bombe` will output all Enigma configurations (with empty plugboards) containing the plaintext
crib string at the given index, and the resulting plaintext. By default, it is fairly slow,
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
0.177778 Rotors: II (F)  VI (Y), V (E) | Reflector: B | Plaintext: HELLONLLON
0.066667 Rotors: III (A)  I (C), V (E) | Reflector: C | Plaintext: HELLOJERSH
0.088889 Rotors: III (A)  II (A), I (A) | Reflector: B | Plaintext: HELLOWORLD # actual plaintext
0.022222 Rotors: III (A)  II (K), I (B) | Reflector: B | Plaintext: HELLOSVRTA
0.088889 Rotors: III (A)  II (Z), I (Z) | Reflector: B | Plaintext: HELLOWORLD
0.066667 Rotors: III (A)  IV (Y), V (K) | Reflector: C | Plaintext: HELLOJJRZH
0.044444 Rotors: III (A)  VI (V), VIII (Q) | Reflector: C | Plaintext: HELLOSHRTF
0.088889 Rotors: III (A)  VI (U), V (X) | Reflector: C | Plaintext: HELLOZHRMH
0.044444 Rotors: III (A)  VI (B), V (Z) | Reflector: C | Plaintext: HELLOOBRXU
0.022222 Rotors: IV (I)  VII (T), VI (E) | Reflector: C | Plaintext: HELLOKYBUA
```

Long ciphertext / short crib results can be sorted through via letter frequencies:

```shell
$ ./bombe -i 0 -c "THIS" -t 64 "QPAYCAAWNNCHFQATVOXQLTCLSVX" | sort -g | tail -1
0.062678 Rotors: IV (D)  I (Z), III (T) | Reflector: C | Plaintext: THISISENCRYPTEDVERYSECURELY
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
$ time ./bombe -i 0 -c "THIS" -t 1 "QPAYCAAWNNCHFQATVOXQLTCLSVX" # 1 thread
real    0m2.980s
user    0m2.893s
sys     0m0.045s
$ time ./bombe -i 0 -c "THIS" -t 64 "QPAYCAAWNNCHFQATVOXQLTCLSVX" # 64 threads
real    0m0.414s
user    0m5.052s
sys     0m0.070s
$ time ./bombe -i 0 -c "THIS" -t 256 "QPAYCAAWNNCHFQATVOXQLTCLSVX" # 256 threads
real    0m0.361s
user    0m5.072s
sys     0m0.078s
```

## enigmacrack (WIP)

Currently a work in progress.

This is a program to crack the Enigma using Index of Coincidence, n-grams, and brute force.
Essentially, we run our ciphertext through each set of rotors and see if the resulting
plaintext looks a bit more like English or whatever language the plaintext should be in,
using letter frequencies and n-grams, or groups of letters. Then, we can do the same thing
for the rotor positions, reflector, and plugboard. This requires a bit more user intervention
but it works around the limited ability of bombe to work with the plugboard. Brute forcing the
Enigma is an extremely computationally expensive endeavor, even with modern technology, so this
is a good way to get around that.

This tool is not fully automated. If no settings are known, you will likely have to run
`enigmacrack` at least once for each component of an Enigma key. The output will contain the most
likely fits, but the top fit may not always be in the actual key.

### Usage

```shell
enigmacrack [options...] method target [file]\n", argv0);
```

The following options are supported:

* `-w rotors`: Set the rotor (Walzen) configuration (e.g. 'I II III')
* `-p positions`: Set the initial rotor positions (e.g. 'ABC')
* `-u reflector`: Set the reflector (Umkehrwalze) (e.g. 'B')
* `-s plugboard`: Set the plugboard (Steckerbrett) configuration (e.g. 'ABCDEF')
* `-S count`: Set the maximum number of plugboard settings (e.g. '10')
* `-c plaintext`: Known plaintext
* `-C position`: Known plaintext index
* `-l language`: Language ('english' or 'german')
* `-t threadCount`: Number of threads to use

method must be one of the following:

* `brute`: Brute force
* `ioc`: Index of Coincidence
* `bigram`: Bigram analysis
* `trigram`: Trigram analysis
* `quadgram`: Quadgram analysis

target must be one of the following:

* `rotors`: Rotors
* `positions`: Rotor positions
* `plugboard`: Plugboard settings
* `reflector`: Reflector

If a file is not given, `enigmacrack` will read from standard input.

## Planned Features

* Bombe should use a dictionary to highlight found plaintext containing more actual words.

## Bugs

* Bombe doesn't support a populated plugboard.
* Probably others.

If you find a bug not listed here, submit an issue, PR, or email me with a
description and/or patch.

## Further Reading

* [The Cryptographic Mathematics of Enigma](https://www.nsa.gov/portals/75/documents/about/cryptologic-heritage/historical-figures-publications/publications/wwii/CryptoMathEnigma_Miller.pdf)
* [Enigma Cipher Machine on Crypto Museum](https://www.cryptomuseum.com/crypto/enigma/index.htm)
* [The Turing-Welchman Bombe (National Museum of Computing)](https://www.tnmoc.org/bombe)
* [Cracking Enigma in 2021 (Computerphile on YouTube)](https://youtu.be/RzWB5jL5RX0)

## License

Copyright (c) 2025 Ben O'Neill <ben@oneill.sh>. This work is released under the
terms of the MIT License. See [LICENSE](LICENSE) for the license terms.
