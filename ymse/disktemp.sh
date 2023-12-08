#!/bin/bash

for disk in $( lsblk -d | awk '/disk/ { print $1 }' ) 
do
    printf "$disk\t"
    temp=$( smartctl -a /dev/$disk | awk '/Temperature_Celsius/ { print $10 }' )
    if $( echo $temp | grep '^[0-9][0-9]*$' > /dev/null 2>&1 )
    then
        echo $temp
    else
        echo 'N/A'
    fi
done
