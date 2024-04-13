#!/bin/bash

#check input
if [ $# -ne 2 ]; then
  echo "Wrong num of input"
  exit 1
fi

politicalPartiesFile="$1"
numLines="$2"

#check if the file exist
if [ ! -f "$politicalPartiesFile" ]; then
  echo "File does not exist: $politicalPartiesFile"
  exit 1
fi

inputFile="inputFile2.txt"
#delete if already exist
rm -f "$inputFile"


partyNames=()  #empty array
#read from file and save to partyNames
while IFS= read -r partyName; do
  partyNames+=("$partyName") #put it in the end of the file
done < "$politicalPartiesFile"


min=3
max=12
arrayLength=${#partyNames[@]}
#create random lines
for ((i=0; i<numLines; i++)); do
  firstName=$(cat /dev/urandom | tr -dc 'a-zA-Z' | fold -w $(shuf -i $min-$max -n 1) | head -n 1)
  lastName=$(cat /dev/urandom | tr -dc 'a-zA-Z' | fold -w $(shuf -i $min-$max -n 1) | head -n 1)

  # choose random party
  randomParty=$((RANDOM % $arrayLength))
  party="${partyNames[$randomParty]}"

  #write line to input file
  echo "$firstName $lastName $party" >> "$inputFile"
done
