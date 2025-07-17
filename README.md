# enigma: Enigma machine simulator and Bombe

This is a CLI M3 Enigma simulator and Bombe.

## Building

```shell
make
sudo make install
```

To enable verbose messages for debugging, uncomment the `ENIGMA_CFLAGS += -DVERBOSE` line in [config.mk](config.mk).

## Usage

### Enigma simulator

This is an almost-full-featured M3 Enigma simulator with rotors I-VIII and Reflectors A-C. It reads from stdin and
outputs to stdout by default, and is case-agnostic.

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
^C
$ ./enigma -p "ABCD" -r "I II IV" -P "XYZ" # same configuration
MWQHY
HELLO
```

### bombe

This is a program based on the machine used to crack Enigma during World War II.
`bombe` takes in 3 arguments: the crib, the index where the crib should be in
the ciphertext, and the ciphertext. The crib is a piece of known plaintext. For example,
during the war, it was common for the Germans to begin their messages with weather reports,
so the German word for "weather report" could be used to crack the message and get the
day's Enigma configuration.

`bombe` will output all Enigma configurations containing the plaintext crib
string at the given index, and the resulting plaintext. An example is below.

```shell
$ ./bombe -c "HELLO" -C "ILBDAAMTAZ" -i 0
Running Bombe...
Crib:
 - Index: 0, String: HELLO
Rotors: II (F)  VI (Y), V (E) | Reflector: B | Plaintext: HELLONLLON
Rotors: III (A)  I (C), V (E) | Reflector: C | Plaintext: HELLOJERSH
Rotors: III (A)  II (A), I (A) | Reflector: B | Plaintext: HELLOWORLD
Rotors: III (A)  II (K), I (B) | Reflector: B | Plaintext: HELLOSVRTA
Rotors: III (A)  IV (Y), V (K) | Reflector: C | Plaintext: HELLOJJRZH
...
```

## Further Reading

* [The Cryptographic Mathematics of Enigma](https://www.nsa.gov/portals/75/documents/about/cryptologic-heritage/historical-figures-publications/publications/wwii/CryptoMathEnigma_Miller.pdf)
* [Enigma Cipher Machine on Crypto Museum](https://www.cryptomuseum.com/crypto/enigma/index.htm)
* [The Turing-Welchman Bombe (National Museum of Computing)](https://www.tnmoc.org/bombe)

## Bugs

* For some reason, GCC optimizations break this code.
* No support for double-stepping rotors (yet).
* Bombe doesn't support a populated plugboard.
* Probably others.

If you find a bug, submit an issue, PR, or email me with a description and/or patch.

## License

Copyright (c) 2025 Ben O'Neill <ben@oneill.sh>. This work is released under the
terms of the MIT License. See [LICENSE](LICENSE) for the license terms.
