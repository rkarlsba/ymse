#!/bin/bash
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker
#
# Written by Roy Sigurd Karlsbakk <roy@karlsbakk.net>
# Licensed under something wierd

for dev in $( lsblk -dn | awk '{ print $1 }' )
do
    c=$( smartctl -a /dev/$dev | grep -w 'Temperature_Celsius' )
    if [ $? -ne 0 ]
    then
        temp="Temperature unavailable"
    else
        temp="$( echo $c | awk '{ print $10 }' )°C"
    fi
    printf "${dev}\t${temp}\n"
done
