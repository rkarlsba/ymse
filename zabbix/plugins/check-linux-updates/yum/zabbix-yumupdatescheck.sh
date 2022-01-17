#!/bin/sh

# Denne sjekker med yum om det trengs oppdateringer. Resultatet lagres i
# TMPFILE, ei midlertidig fil. Hvis skriptet kalles uten parametre, kjører det
# yum direkte. Ulempa med dette, er at yum kan bruke lovlig lang tid på sjekken
# (>10s), så før den er ferdig, har Zabbix fått tidsavbrudd. Ei løsning på
# dette, er å kjøre sjekken i cron og lagre resultatet et sted, her OUTFILE. For
# at den skal lagre dit, kjøres skriptet med --cron. Når så Zabbix skal kjøre
# testen, kjøres skriptet med --local for å unngå å kjøre yum i sanntid, men
# heller lagre de eldre dataene. Frekvensen til cron-jobben er opp til den
# enkelte admin, men jeg vil tru en gang om dagen vil duge.

export LANG=C

TMPFILE=$( /usr/bin/mktemp /tmp/zabbix-checkupdates.XXXXX )
CHECKNAME='custom.yumupdatescheck'
OUTFILE='/var/run/zabbix-yumupdatescheck'

STATUS='WARN'

if [ -x /usr/bin/yum ]
then
    if [ "$1" == "--local" ]
    then
        cat $OUTFILE > $TMPFILE
    else
        /usr/bin/yum check-update > $TMPFILE 2>&1
    fi
    EXIT="$?"

    [ "$EXIT" == "0" ] && STATUS='OK'
    /usr/bin/egrep -q -i '^(Error|Cannot)' $TMPFILE && STATUS="FAIL"
else
    STATUS='OK'
fi

if [ "$1" == "--cron" ]
then
    echo $CHECKNAME $STATUS > $OUTFILE
    [ "$STATUS" == "OK" ] || /usr/bin/cat $TMPFILE >> $OUTFILE
else
    echo $CHECKNAME $STATUS
    [ "$STATUS" == "OK" ] || /usr/bin/cat $TMPFILE
fi

/usr/bin/rm -f $TMPFILE
