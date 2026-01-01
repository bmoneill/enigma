#!/bin/bash
# ngram generator
# usage: genngrams n file
# outputs ngram counts to stdout.
# first line of output is "n charcount"

if [[ $# -ne 2 ]]; then
  echo "Usage: $0 n file"
  exit 1
fi

if [[ $1 -lt 1 || $1 -gt 4 ]]; then
  echo "Error: first argument must be an integer between 1 and 4"
  exit 1
fi

# Check if file exists
if [ ! -f "$2" ]; then
  echo "Error: file \"$2\" not found"
  exit 1
fi

n=$1
f=$2
l=$(wc -c < $f)
echo "$n $l"
cat $f | tr ' ' '\n' | tr -cd '[:alpha:] \n' | tr '[:upper:]' '[:lower:]' | awk -v n=$n -F'\n' '{
  for(i=1; i<=NF; i++) {
    for(i=1; i<=length($0)-n+1; i++) {
      print substr($0, i, n)
    }
  }
}' | sort | uniq -c | sort -nr | sed 's/^[ \t]*//'
