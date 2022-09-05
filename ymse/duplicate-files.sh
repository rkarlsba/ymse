#!/bin/bash
# vim:ts=4:sw=4:sts=4:et:ai

src='rand.orig'
dst='base/rand.0x%04x'
nn='noe fint snart'

trap sigint INT

function sigint() {
    echo
    echo "Removing temporary file $nn"
    rm $nn
    exit 0
}

#for ((count = 0 ; count < 20 ; count++))
for ((count = 0 ; count < 1024 ; count++))
do
    nn=$( printf $dst $count )
    if [ ! -f $nn -o -z $nn ]
    then
        echo -n "cp $src $nn:"
        cp $src $nn
        exitcode=$?
        if [ $exitcode -ne 0 ]
        then
            echo
            echo "cp returned $exitcode - exiting"
            exit $exitcode
        else
            echo " OK"
        fi
    fi
done
