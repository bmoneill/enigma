<h1 align="center">
  indexgen
</h1>

<h4 align="center">
  A rotor index generator for libenigma.
</h4>

## Usage

```shell
indexgen.py rotor_alphabet
```

Given a rotor alphabet, this tool generates forward and reverse index mappings
(EnigmaRotor.fwd_indices and EnigmaRotor.rev_indices), given a rotor's standard
alphabet. This is useful for implementing custom or obscure Enigma rotors. To utilize
these mappings, you need to modify libenigma - there is no way to directly use them
in `enigmacli` or `enigmacrack`.
