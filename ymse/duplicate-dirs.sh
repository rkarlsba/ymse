#!/bin/bash
# vim:ts=4:sw=4:sts=4:et:ai

rsync_opts='-a --partial'
src='base'
dst='dir.0x%02x'
nn='noe fint snart'

trap sigint INT

function sigint() {
    echo
    echo "Interrupted - dunno what to do"
    exit 0
}

for ((count = 0 ; count < 32 ; count++))
do
    nn=$( printf $dst $count )
    echo -n "rsync $rsync_opts $src $nn:"
    rsync $rsync_opts "$src/" $nn
    exitcode=$?
    if [ $exitcode -ne 0 ]
    then
        echo
        echo "rsync returned $exitcode - exiting"
        exit $exitcode
    else
        echo " OK"
    fi
done
