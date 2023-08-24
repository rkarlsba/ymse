#!/bin/bash
#Prepared by Marcin 'szremo' Szremski'
SSL_DATE=$(echo | openssl s_client -servername $1 -connect $1:443 2>/dev/null | openssl x509 -noout -dates | grep notAfter | awk -F = '{ print $2 }' | awk 'BEGIN {OFS = "-"} {print $1,$2,$4}')
EXPTIME=$(date -d$SSL_DATE +%s)
TODAY=$(date +"%b-%d-%Y")
CURRENT=$(date -d$TODAY +%s)
NUM=$(($EXPTIME - $CURRENT))
RESULT=$(($NUM/86400))
echo $RESULT
