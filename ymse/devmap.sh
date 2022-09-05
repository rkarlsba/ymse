#!/bin/bash
# vim:ts=4:sw=4:sts=4:ai:et
# inspired by http://askubuntu.com/questions/64351/how-to-match-ata4-00-to-the-apropriate-dev-sdx-or-actual-physical-disk

VERBOSE=0

if [ "$1" = "-v" ]
then
    VERBOSE=1
fi

os=$( uname )
if [ "$os" != "Linux" ]; then
    echo "This script works on Linux only" >&2
    exit 1
fi

for d in /sys/block/sd*
do
    s=$( basename $d )
    h=$( ls -l $d | egrep -o "host[0-9]+" )
    t=$( ls -l $d | egrep -o "target[0-9:]*" )
    a2=$( echo $t | egrep -o "[0-9]:[0-9]$" | sed 's/://' )
    lookuppath="/sys/class/scsi_host/$h"
    a=$( cat $lookuppath/unique_id )
    if [ $VERBOSE -gt 0 ]
    then
        echo "$s -> ata$a.$a2 ($lookuppath/$a2)"
    else
        echo "$s -> ata$a.$a2"
    fi
done
