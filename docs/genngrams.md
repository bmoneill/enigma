<h1 align="center">
  genngrams
</h1>

<h4 align="center">
  An N-Gram Generator
</h4>

## Usage

```shell
genngrams n file
```

This script generates n-grams from a given file. The first option is the n-gram
size, and the second is the file to read from.

## Output

Output is formatted like so:

```plaintext
n charcount
count ngram
count ngram
...
```

Where:

- `n` is the number of characters in each N-Gram,
- `charcount` is the total number of characters in the input text,
- `count` is the number of occurances of the given N-Gram,
- and `ngram` is the N-Gram value.

The output can be directly used by `libenigma` and `enigmacrack`.
