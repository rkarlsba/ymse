#!/bin/sh
# vim:ts=4:sw=4:et:ai:fdm=marker:tw=80

# Written by Roy Sigurd Karlsbakk <roysk@oslomet.no> for use with Zabbix.

# If server for some reason uses another locale, locally switch it to C for now
export LANG=C 

# If --local is given, run a local check, that
if [ "$1"=="--local" ]
then
    LOCAL=1
else
    LOCAL=0
fi

TMPFILE=$(mktemp /tmp/zabbix-checkupdates.XXXXX)
CHECKNAME='custom.yumupdatescheck'
OUTFILE='/var/tmp/zabbix-yumupdatescheck'

STATUS='WARN'

# RHEL/Centos check
if $( which yum > /dev/null 2>&1 )
then
    yum check-update > $TMPFILE 2>&1
    EXIT="$?"

    if $( egrep -q -i '^(Error|Cannot)' $TMPFILE )
    then
        STATUS="FAIL"
    elif [ "$EXIT" = "0" ]
    then
        STATUS='OK'
    fi
fi

if [ $LOCAL ]
then
    echo $CHECKNAME $STATUS > $OUTFILE
    cat $TMPFILE > $OUTFILE
else
    echo $CHECKNAME $STATUS
    cat $TMPFILE
fi

rm -f $TMPFILE

