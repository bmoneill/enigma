# indexgen: Rotor Index Generator

## Usage

```shell
indexgen.py rotor_alphabet
```

Given a rotor alphabet, this tool generates forward and reverse index mappings
(EnigmaRotor.fwd_indices and EnigmaRotor.rev_indices), given a rotor's alphabet.
This is useful for implementing custom or obscure Enigma rotors. To utilize
these mappings, you need to use libenigma - there is no way to directly use them
in `enigmacli` or `enigmacrack`.
