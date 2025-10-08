# enigmacrack: Cryptanalysis tools to break Enigma ciphers

## Usage

```shell
enigmacrack method [target] [options] ciphertext
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
* `-m float`: Set the minimum score threshold.
* `-M float`: Set the maximum score threshold.
* `-n file`: Load n-grams from the given file.
* `-S count`: Set the maximum number of plugboard settings to account for (default: 8).
* `-t count`: Set the number of threads to use (default: 8).

### Other Options

* `-h`: Print a help string
* `-v`: Show verbose output

## Methods

### brute

Use brute force for cryptanalysis. This method runs through every possible Enigma configuration and
corresponding plaintext against a dictionary. This is *very* slow, even with today's
hardware capabilities (there are around **159 quintillion** possible Enigma configurations).

If a dictionary file was set, plaintexts will be compared against the file and only results matching
multiple dictionary words will be outputted.

If a dictionary file is not set, it will use letter frequency scores to decide if a plaintext is
worth analyzing. The `-m` and `-M` parameters may be used to set minimum and maximum letter
frequency scores.

### ioc

**Options required: `target`, `-m/-M/-T` OR `-l`.**

This method uses Index of Coincidence for cryptanalysis. The Index of Coincidence measures how
likely it is to draw two random letters from a given text. As correct settings are discovered,
intermediary plaintexts should approach the established Index of Coincidence value for its language.

Utilizing this method, it is possible to decrypt ciphertext with no known settings in a short amount of time,
given we know the language the source text was written in. This requires some user intervention, however.

### ngram

**Options required: `target`, `-n`, `-m/-M/-T`.**

Use n-grams for cryptanalysis. n-grams may be generated from a corpus using the [ngram.sh](../tools/ngram.sh)
script.

## Targets

* `rotors[1-3]`: Crack the rotor (Walzen) configuration. Optional index indicates which rotor to crack.
* `positions[1-3]`: Crack the initial rotor positions. Optional index indicates which rotor to crack positions of.
* `reflector`: Crack the reflector (Umkehrwalze).
* `plugboard`: Crack the plugboard (Steckerbrett) configuration.
