# Enigma machine simulator and Bombe

This is a CLI M3 Enigma simulator and Bombe.

## Building

```shell
make
sudo make install
```

To enable verbose messages for debugging, uncomment the `ENIGMA_CFLAGS += -DVERBOSE` line in [config.mk](config.mk).

## Enigma Simulator

This is an M3 Enigma simulator with rotors I-VIII and Reflectors A-C. It reads from stdin and
outputs to stdout by default, and is case-agnostic.

The Enigma was a cipher device used by the Germans during World War II to encrypt military communication.
It uses a very complex wiring system to scramble the input.

When a key is pressed, the signal first passes through the plugboard, which swaps the letter with another according
to its configuration. It then travels through three rotors, each further scrambling the signal. Next, the signal
reaches the reflector, which adds another layer of complexity by sending it back through the rotors in reverse order.
Finally, the signal passes through the plugboard again, and the resulting encoded letter lights up on the lampboard.

The Enigma's encryption was symmetric, meaning that using the same configuration for both encryption and decryption
allowed the ciphertext to be converted back to plaintext simply by typing it in.

Needless to say, this was pretty strong cryptography for the time, and it was a top priority of the Allies to break
Enigma.

The following options are supported:

* `-p`: Set the plugboard configuration (e.g. 'ABCDEF')
* `-P`: Set the initial position of the rotors (e.g. 'ABC')
* `-r`: Set the rotor configuration (e.g. 'I II III')
* `-R`: Set the reflector (e.g. 'B')

Here is an example encryption/decryption with a custom rotor/plugboard configuration:

```shell
$ ./enigma -p "ABCD" -r "I II IV" -P "XYZ"
HELLO # user input
MWQHY # output
$ ./enigma -p "ABCD" -r "I II IV" -P "XYZ" # same configuration
MWQHY
HELLO
```

## Bombe

This is a program based on the machine used to crack Enigma during World War II.
`bombe` takes in 3 arguments: the crib, the index where the crib should be in
the ciphertext, and the ciphertext. The crib is a piece of known plaintext. For example,
during the war, it was common for the Germans to begin their messages with weather reports,
so the German for "weather report" could be used to crack the message and get the
day's Enigma configuration.

`bombe` will output all Enigma configurations containing the plaintext crib
string at the given index, and the resulting plaintext. By default, it is fairly slow,
so multithreading is supported.

### Usage

The following options are supported:

* `-c`: Crib string (required)
* `-C`: Ciphertext (required)
* `-i`: Crib string index in plaintext (required)
* `-t`: Number of threads to use (default: 1)

### Example

```shell
$ ./bombe -i 0 -c "HELLO" -C "ILBDAAMTAZ" -t32
Rotors: II (F)  VI (Y), V (E) | Reflector: B | Plaintext: HELLONLLON
Rotors: III (A)  II (A), I (A) | Reflector: B | Plaintext: HELLOWORLD
Rotors: III (A)  I (C), V (E) | Reflector: C | Plaintext: HELLOJERSH
Rotors: III (A)  II (K), I (B) | Reflector: B | Plaintext: HELLOSVRTA
Rotors: III (A)  IV (Y), V (K) | Reflector: C | Plaintext: HELLOJJRZH
Rotors: III (A)  VI (V), VIII (Q) | Reflector: C | Plaintext: HELLOSHRTF
Rotors: III (A)  VI (U), V (X) | Reflector: C | Plaintext: HELLOZHRMH
Rotors: III (A)  VI (B), V (Z) | Reflector: C | Plaintext: HELLOOBRXU
Rotors: III (A)  VIII (P), VII (T) | Reflector: A | Plaintext: HELLOXCRUE
```

### Multithreading

Multithreading reduces the runtime by a significant margin. I'm sure this
code could be optimized much further (the Enigma encoding function focuses
on readability rather than performance - this is not ideal for the Bombe).

Below are single- and multi-threaded times on my 16-core i9-10885H.

```shell
$ time ./bombe -i 0 -c "HELLO" -C "ILBDAAMTAZ" -t 1 # single thread
real    0m15.231s
user    0m14.980s
sys     0m0.117s
$ time ./bombe -i 0 -c "HELLO" -C "ILBDAAMTAZ" -t 32 # 32 threads
real    0m1.657s
user    0m20.785s
sys     0m0.118s
```

## Planned Features

* Bombe should be able to find potential crib indices (WIP).

## Bugs

* For some reason, GCC optimizations break this code.
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
