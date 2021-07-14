#!/bin/bash
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker

TMPFILE=$(mktemp /tmp/tmptest.XXXXX)

function beforeidie {
    echo "AAAAAArrrgghh! They'll never take me alive!!!!!" >&2
    rm -f $TMPFILE
}

trap beforeidie EXIT

touch $TMPFILE

exit
