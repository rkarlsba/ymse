#!/bin/bash
# vim:ts=4:sw=4:sts=4:et:ai

# Søk etter log4j i alle jar-filer. Forutsetter at locate-databasen er 
# oppdatert. På grunn av problematikk med mellomrom i katalog og filnavn 
# legges listen over jar-filer i en temporær fil.

# se man mktemp (eller trykk K på kommandoen her i vim for å fyre opp manuelen for den)
MKTMPTEMP="/tmp/log4jsearch.XXXXX"
REGEX="log4j\.jar|log4j-[0-9].*\.jar$|log4j-core.*\.jar$"
OUTFILE=$( mktemp $MKTMPTEMP )
JARLIST=$( mktemp $MKTMPTEMP )
QUIET=0
HITS=0
STATUS='OK'
STATUSTEXT=''

# Funksjon som kalles ved avslutning av skriptet.
function cleanup() {
    rm -f $OUTFILE $JARLIST
}

# Kall funksjonen cleanup når skriptet avsluttes, med mindre det får en SIGKILL - den når aldri skriptet.
trap cleanup EXIT

if [ $UID -gt 0 ]
then
    if [ $QUIET -eq 0 ]
    then
        STATUS="WARNING"
        STATUSTEXT="Running under a non-root user. This may not find all files. "
    fi
fi

# Sjekk om vi har locate og bruk den hvis vi har den.
if $( which locate > /dev/null 2>&1 )
then
    USE_LOCATE=1
else
    USE_LOCATE=0
fi

if [ $USE_LOCATE -gt 0 ]
then
    locate -r "\.jar$" > $JARLIST
else
    if [ $QUIET -eq 0 ]
    then
        echo "Warning: command 'locate' not found. Falling back to 'find', which may" >&2
        echo "time out Zabbix checks." >&2
    fi
    find / -iname '*.jar' -print > $JARLIST 2>/dev/null
fi

while read line
do
    HIT=`unzip -l "$line" 2>/dev/null | egrep -i "$REGEX"`
    if [ -n "$HIT" ]; then
        HITS=$( $HITS + 1 )
        echo -e "$line\n$HIT\n" >> $OUTFILE
    fi
done < $JARLIST

if [ $HITS -gt 0 ]
then
    echo "ERROR: Found a total of $HITS hits on log4j search:"
    cat $OUTFILE
    exit 1
else
    STATUSTEXT+="log4j not found on this machine."
    echo "$STATUS: $STATUSTEXT"
fi

exit 0

