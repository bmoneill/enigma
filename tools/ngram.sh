#!/bin/bash
# ngram generator
# usage: ngram.sh n file
# outputs ngram counts to stdout.
# first line of output is "n linecount"

n=$1
f=$2
l=$(wc -l < $f)
echo "$n $l"
cat $f | tr ' ' '\n' | tr -cd '[:alpha:] \n' | tr '[:upper:]' '[:lower:]' | awk -v n=$n -F'\n' '{
  for(i=1; i<=NF; i++) {
    for(i=1; i<=length($0)-n+1; i++) {
      print substr($0, i, n)
    }
  }
}' | sort | uniq -c | sort -nr | sed 's/^[ \t]*//'
