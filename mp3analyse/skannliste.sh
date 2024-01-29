#!/bin/bash

for fn in $@
do
    while read -r datafile
    do
        ./iszeroed "$datafile"
    done < $fn
done
