#!/bin/bash
# vim:ts=4:sw=4:sts=4:et:ai

date=$( date -I )
url="https://www.tu.no/api/widgets/comics?name=lunch&date=$date"
homedir="/somewhere/Lunch"
fn="$homedir/lunch-tu-$date.jpg"
progname=$0

# Code
logger "$progname started"

if [ ! -s $fn ]
then
    logger "$progname Downloading $url to $fn"
    wget -qO "$fn" "$url"
else
    logger "$progname finished without downloading"
fi

