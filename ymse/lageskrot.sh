#!/bin/bash

IF="/dev/urandom"
OF_NAME="skrot"
OF_MAL="$OF_NAME.%d"
JOBBER=1

echo "Denne jobben vil fylle opp $PWD med skrot, filer ved navn $OF_NAME.noe"
echo "Den tar ikke hensyn til noe annet og er laget for testing."
echo
echo -n "Vil du fortsette? (j/N) "

read answer
answer=$( echo $answer | tr [A-Z] [a-z] )
if [ "$answer" == "y" -o "$answer" == "j" ]
then
        echo "Da kjører vi på :)"
else
        echo "Da dreit vi i det - ha en fin dag :)"
        exit 0
fi

for i in $( seq 1 $jobber )
do
        OF=$( printf $OF_MAL $i )
        echo "Lager skrotfil \"$OF\""
        dd if=$IF of=$OF bs=4096 status=progress
done
