#!/bin/bash
# vim:ts=4:sw=4:sts=4:et:ai:si:fdm=marker
#
# Written for json files - change as you please
#
# Not licensed to anything, say, BSD license, then
#
# Roy Sigurd Karlsbakk <roy@karlsbakk.net>

if [[ "$1" == "-v" ]]
then
    VERBOSE=1
elif [[ "$1" == "-q" ]]
then
    QUIET=1
fi

for f in $( find . -maxdepth 1 -type f -name \*.json -size 0 )
do
    if $( lsof $f > /dev/null 2>&1 )
    then
        [[ $QUIET == 1 ]] || echo "File $f is open, ignoring"
    else
        [[ $VERBOSE == 1 ]] && echo "Remove file $f"
        rm -f $f
    fi
done
