# enigmacrack: Cryptanalysis tools to break Enigma ciphers

## Usage

```shell
enigmacrack method [target] [options] [ciphertext]
```

## Methods

### bombe

Options required: `-c`, `-C`.

Use a pseudo-Bombe for cryptanalysis. This assumes an empty plugboard.

### brute (WIP)

Use brute force for cryptanalysis. This method runs through every possible Enigma configuration and
matches the corresponding plaintext against a dictionary. This is *very* slow, even with today's
hardware capabilities (there are around **159 quintillion** possible Enigma configurations).

### ioc (WIP)

Options required: `target`, `-m/-M` OR `-l`

Use Index of Coincidence for cryptanalysis.

### ngram (WIP)

Options required: `target`, `-n`, `-m/-M`

Use n-grams for cryptanalysis.

## Targets

* `rotors`: Crack the rotor (Walzen) configuraiton
* `positions`: Crack the initial rotor positions.
* `reflector`: Crack the reflector (Umkehrwalze).
* `plugboard`: Crack the plugboard (Steckerbrett) configuration.

## Options

### Enigma Settings

* `-w rotors`: Set the rotor configuration (e.g. 'I II III')
* `-p positions`: Set the initial rotor positions (e.g. 'ABC')
* `-u reflector`: Set the reflector (e.g. 'B')
* `-s plugboard`: Set the plugboard pairs (e.g. 'ABCDEF')

### Cryptanalysis Settings

* `-c plaintext`: Set the known plaintext
* `-C position`: Set the position of known plaintext
* `-l language`: Set the language ('english' or 'german', for IOC method)
* `-m float`: Set the minimum score threshold (for n-gram and IOC methods)
* `-M float`: Set the maximum score threshold (for n-gram and IOC methods)
* `-n file`: Load n-grams from the given file
* `-S count`: Set the maximum number of plugboard settings to account for (default: 8)
* `-t count`: Set the number of threads to use (default: 8)

### Other Options

* `-h`: Print a help string
* `-v`: Show verbose output
