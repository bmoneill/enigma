# enigmacrack: Cryptanalysis tools to break Enigma ciphers

## Usage

```shell
enigmacrack method target [options] ciphertext
```

## Options

### Enigma Settings

* `-w rotors`: Set the rotor configuration (e.g. 'I II III').
* `-p positions`: Set the initial rotor positions (e.g. 'ABC').
* `-u reflector`: Set the reflector (e.g. 'B').
* `-s plugboard`: Set the plugboard pairs (e.g. 'ABCDEF').

### Cryptanalysis Settings

* `-c plaintext`: Set the known plaintext.
* `-C position`: Set the position of known plaintext.
* `-d path`: Load dictionary words from the given file.
* `-l language`: Set the language ('english' or 'german', for IOC method).
* `-m float`: Set the minimum score threshold (**REQUIRED**).
* `-M float`: Set the maximum score threshold (**REQUIRED**).
* `-n file`: Load n-grams from the given file.

### Other Options

* `-h`: Print a help string
* `-v`: Show verbose output

## Methods

### ioc

This method uses Index of Coincidence for cryptanalysis. The Index of Coincidence measures how
likely it is to draw two random letters from a given text. As correct settings are discovered,
intermediary plaintexts should approach the established Index of Coincidence value for its language.

Utilizing this method, it is possible to decrypt ciphertext with no known settings in a short amount of time,
given we know the language the source text was written in. This requires some user intervention, however.

### ngram

Use n-grams for cryptanalysis. n-grams may be generated from a corpus using the [ngram.sh](../tools/ngram.sh)
script.

## Targets

* `rotor[1-3]`: Crack the rotor (Walzen) configuration. Index indicates which rotor to crack.
* `positions`: Crack the initial rotor positions.
* `reflector`: Crack the reflector (Umkehrwalze).
* `plugboard`: Crack a plugboard (Steckerbrett) setting.

