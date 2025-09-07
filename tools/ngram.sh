#!/bin/sh
# ngram generator
# usage: ngram.sh n < file

n=$1
while read line; do
echo $line | tr ' ' '\n' | tr -cd '[:alpha:] \n' | tr '[:upper:]' '[:lower:]' | awk -v n=$n -F'\n' '{
  for(i=1; i<=NF; i++) {
    for(i=1; i<=length($0)-n+1; i++) {
      print substr($0, i, n)
    }
  }
}'
done | sort | uniq -c | sort -nr
