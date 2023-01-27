#!/bin/bash
# vim:ts=4:sw=4:sts=4:et:ai

oui='oui.txt'
export TMPDIR=/tmp
fn=$( mktemp )
wget -O $fn http://standards-oui.ieee.org/oui.txt
if [ $? -eq 0 ]
then
    if [ `diff -u $fn $oui` ]
    then
        echo "New and old files match"
        exit
    fi
    mv -f $oui $oui-$( date -I )
    mv $fn $oui
fi
