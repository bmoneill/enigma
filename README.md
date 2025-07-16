# enigma: Enigma machine simulator and Bombe

This is a CLI M3 Enigma simulator and Bombe.

## Building

```shell
make
sudo make install
```

To enable verbose messages for debugging, uncomment the `CFLAGS += -DVERBOSE` line in [config.mk](config.mk).

## Usage

### enigma

`enigma` will read input from stdin and print to stdout. Redirection is up to the user.
Parameters are listed below. `enigma` works with both uppercase and lowercase input.

```shell
$ ./enigma -h
./enigma: invalid option -- 'h'
Usage: ./enigma [-p plugboard] [-r rotors] [-R reflector] [-P position]
Options:
  -p plugboard   Set the plugboard configuration (e.g., 'AB CD EF')
  -P position    Set the initial position of the rotors (e.g., 'ABC')
  -r rotors      Set the rotor configuration (e.g., 'I II III')
  -R reflector   Set the reflector configuration (e.g., 'B')
Available rotors: I, II, III, IV, V, VI, VII, VIII
Available reflectors: A, B, C
$ ./enigma
HELLO
ILBDA
^C
$ ./enigma
ILBDA
HELLO
^C
$
```

### bombe

`bombe` takes in 3 arguments: the crib, the index where the crib should be in
the ciphertext, and the ciphertext. It will output all Enigma configurations containing
the plaintext crib at the given index, and the entire plaintext.

```shell
$ ./bombe -h
./bombe: invalid option -- 'h'
Usage: ./bombe -c crib -i index -p ciphertext
Options:
  -c crib       Set the crib string to use
  -C ciphertext Set the ciphertext to analyze
  -i index      Set the index where the crib string should be in the ciphertext
$ ./bombe -c HELLO -i 0 -C ILBDA
Running Bombe...
Crib:
 - Index: 0, String: HELLO
Rotors: II (F)  VI (Y), V (E) | Reflector: B | Plaintext: HELLO
Rotors: III (A)  I (C), V (E) | Reflector: C | Plaintext: HELLO
Rotors: III (A)  II (A), I (A) | Reflector: B | Plaintext: HELLO
Rotors: III (A)  II (K), I (B) | Reflector: B | Plaintext: HELLO
Rotors: III (A)  IV (Y), V (K) | Reflector: C | Plaintext: HELLO
...
```

## Further Reading

* [The Cryptographic Mathematics of Enigma](https://www.nsa.gov/portals/75/documents/about/cryptologic-heritage/historical-figures-publications/publications/wwii/CryptoMathEnigma_Miller.pdf)
* [Enigma Cipher Machine on Crypto Museum](https://www.cryptomuseum.com/crypto/enigma/index.htm)

## Bugs

* For some reason, GCC optimizations break this code.
* No support for double-stepping rotors (yet).
* Bombe doesn't support a populated plugboard.
* Probably others.

If you find a bug, submit an issue, PR, or email me with a description and/or patch.

## License

Copyright (c) 2025 Ben O'Neill <ben@oneill.sh>. This work is released under the
terms of the MIT License. See [LICENSE](LICENSE) for the license terms.
