# enigmacrack: Cryptanalysis tools to break Enigma ciphers

## Usage

```shell
enigmacrack method [target] [options] [ciphertext]
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

If a dictionary file is not set, it will use letter frequency scores to decide if a plaintext is worth
analyzing. The `-m` and `-M` parameters may be used to set minimum and maximum letter frequency scores.

### ioc

Options required: `target`, `-m/-M/-T` OR `-l`

Use Index of Coincidence for cryptanalysis.

### ngram

Options required: `target`, `-n`, `-m/-M/-T`

Use n-grams for cryptanalysis.

## Targets

* `rotors`: Crack the rotor (Walzen) configuration.
* `positions`: Crack the initial rotor positions.
* `reflector`: Crack the reflector (Umkehrwalze).
* `plugboard`: Crack the plugboard (Steckerbrett) configuration.
