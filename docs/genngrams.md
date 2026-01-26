# genngrams: N-Gram Generator

## Usage

```shell
genngrams n file
```

This script generates n-grams from a given file. The first option is the n-gram
size, and the second is the file to read from. The first line of output is "n charcount",
and the subsequent lines are the n-grams themselves (formatted as "count ngram").
The output can be directly used by `libenigma` and `enigmacrack`.
