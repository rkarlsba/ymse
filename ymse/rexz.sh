#!/bin/bash
# vim:ts=4:sw=4:sts=4:et:ai

function knownfile {
	f=$1
	ext="${f##*.}"
	echo $ext
}

for f in $@
do
	knownfile $f
done
