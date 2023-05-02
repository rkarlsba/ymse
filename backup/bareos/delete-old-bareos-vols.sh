#!/bin/bash

DATADIR="/data/bareos"
SECS=3
fuckem=0

if [ $UID -ne 0 ]; then
	echo "Only root wants to run this" >&2
	exit 1
fi

if [ "$1" == "--fuckem" ]; then
	fuckem=1
fi

cd $DATADIR || exit

VOLS_TO_PURGE=$( list-vols-ret-status.pl |awk '/EXPI/ { print $6 }' )
NO_VOLS_TO_PURGE=$( echo $VOLS_TO_PURGE  |wc -w )

if [ $NO_VOLS_TO_PURGE == 0 ] 
then
    echo "No purgable volumes found"
    exit 0
fi

if [ $fuckem == 0 ]
then
	echo
	echo "This will purge/delete all $NO_VOLS_TO_PURGE of the following volumes from bareos"
	echo "and remove the datafiles residing under $DATADIR. "

	echo $VOLS_TO_PURGE

	echo -n "Are you sure you want to continue? (y/N) "
	read answer
	if [ "$answer" != "y" -a "$answer" != "Y" ]; then
		echo "Nothing done"
		exit
	fi

	echo -n "Deleting things - press ctrl+c to abort within $SECS seconds"
	sleep $SECS
	echo " OK"
fi

# Deleting things
for vol in $VOLS_TO_PURGE
do
	if [ $fuckem == 0 ]
	then
		echo "prune yes volume=$vol" | bconsole > /dev/null
		echo "delete yes volume=$vol" | bconsole > /dev/null
	else
		echo "prune yes volume=$vol" | bconsole
		echo "delete yes volume=$vol" | bconsole
		echo "DELETING FILE $vol"
	fi
	rm $vol
done
