#!/bin/bash

basedn="dc=oslomet,dc=no"

max=0
for s in $( cat ldap-servere.txt )
do
    m=$( echo $s | wc -c )
    if [ $m -gt $max ]
    then
        max=$m
    fi
done

max=$(( $max + 1 ))

for s in $( cat ldap-servere.txt ) 
do
    url="ldaps://$s"
    cmd="./zabbix_check_ldap.pl -b $basedn -U $url"
    result=$( $cmd )
    printf "%-*s%s\n" "$max" "$s" "$result"
done
