#!/bin/bash
# vim:ts=4:sw=4:sts=4:et:ai

INFILE='/dev/sdd'
OUTFILE='outfile'
OUT_IMG="$OUTFILE.img"
OUT_MAP="$OUTFILE.map"

if [ $EUID -gt 0 ]
then
    echo "Run me ass root, please" >&2
    exit 1
fi

FILEEXISTS=0

if [ -f $OUT_IMG ]
then
    FILEEXISTS=$(( $FILEEXISTS + 1 ))
    echo "Output image file '$OUT_IMG' exists already" >&2
fi
if [ -f $OUT_MAP ]
then
    FILEEXISTS=$(( $FILEEXISTS + 1 ))
    echo "Output map file '$OUT_MAP' exists already" >&2
fi
if [ $FILEEXISTS -gt 0 ]
then
    echo "Exiting" >&2
    exit 2
fi
ddrescue -v -v $INFILE $OUT_IMG $OUT_MAP
