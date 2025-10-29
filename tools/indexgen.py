#!/usr/bin/env python3
# Generate index tables for a given rotor alphabet
# Usage: python indexgen.py <rotor_alphabet>

import sys


def toi(c):
    return ord(c.upper()) - ord("A")


def generate_indexes(rotalpha):
    if len(rotalpha) != 26:
        raise ValueError("Rotor alphabet must be 26 characters long")

    alpha = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    fwd = []
    rev = [-1] * 26
    for i in range(len(alpha)):
        fwdidx = toi(rotalpha[i])
        fwd.append(fwdidx)

    for c in alpha:
        for i in range(len(rotalpha)):
            if rotalpha[i] == c:
                rev[toi(c)] = i

    return fwd, rev


def main():
    if len(sys.argv) != 2:
        print("Usage: %s <rotor_alphabet>" % sys.argv[0])
        return
    fwd, rev = generate_indexes(sys.argv[1])
    print(fwd)
    print(rev)


if __name__ == "__main__":
    main()
