#!/bin/bash

inputFile.txt="inputFile.txt"
tallyResultsFile="$1"

#check if the file exists
if [ ! -f "$inputFile" ]; then
  echo "Does not exist"
  exit 1
fi

#Check read permissions of inputFile.txt
if [ ! -r "$inputFile" ]; then
  echo "No read permissions"
  exit 1
fi

declare -A voteCounts
declare -A names

while IFS= read -r line; do
  #save the first and second word
  name=$(awk '{ print $1 }' <<< "$line")
  surname=$(awk '{ print $2 }' <<< "$line")
  voter="$name $surname"
  #checks if the voter is not already registered in the names table
  if [[ -z "${names[$voter]}" ]]; then
    #save the third word
    party=$(awk '{ print $3 }' <<< "$line")
    #if party does not exist in voterCounts
    if [[ ! -v "${voteCounts[$party]}" ]]; then
      ((voteCounts[$party]++))
    fi
    names["$voter"]=1
  fi
done < "$inputFile"


#write to tallyResultsFile
> "$tallyResultsFile"
for key in "${!voteCounts[@]}"; do
  echo "$key ${voteCounts[$key]}" >> "$tallyResultsFile"
done



