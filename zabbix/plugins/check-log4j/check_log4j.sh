#!/bin/bash
# vim:ts=4:sw=4:sts=4:et:ai

# See mktemp(1)
MKTMPTEMP="/tmp/log4jsearch.XXXXX"
REGEX="log4j\.jar|log4j-[0-9].*\.jar$|log4j-core.*\.jar$"
OUTFILE=$( mktemp $MKTMPTEMP )
JARLIST=$( mktemp $MKTMPTEMP )
QUIET=0
HITS=0
STATUS='OK'
STATUSTEXT=''
USE_LOCATE=0
OS=$( uname )

# This is called on exit
function cleanup() {
    rm -f $OUTFILE $JARLIST
}

# Call this function on exit (except if we get SIGKILL, then we won't see it)
trap cleanup EXIT

if [ $UID -gt 0 ]
then
    STATUS="WARNING"
    STATUSTEXT="Running under a non-root user. This may not find all files. "
fi

# Check if we have locate
if $( which locate > /dev/null 2>&1 )
then
    # We need gnu locate, so we'll stick to checking if this is Linux or not
    if [ "$OS" == "Linux" ]
    then
        USE_LOCATE=1
    fi
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

