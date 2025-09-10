# enigmacli

This is a CLI M3 Enigma simulator.

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
* `-r`: Generate a random Enigma configuration.

Here is an example encryption/decryption with a custom rotor/plugboard configuration:

```shell
$ enigmacli -s "ABCD" -w "I II IV" -u "B" -p "XYZ"
HELLO # user input
MWQHY # output
$ enigmacli -s "ABCD" -w "I II IV" -u "B" -p "XYZ" # same configuration
MWQHY
HELLO
```
