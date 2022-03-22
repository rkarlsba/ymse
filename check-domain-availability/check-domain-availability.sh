#!/bin/bash 
# vim:ts=4:sw=4:sts=4:et:ai
 
# Name: Check for domain name availability 
# linuxconfig.org 
# Please copy, share, redistribute and improve 

checkdomain() {
    whois $1 | egrep -q \
        '^% No match|^No match|^NOT FOUND|^Not fo|AVAILABLE|^No Data Fou|has not been regi|No entri'
    if [ $? -eq 0 ]
    then
        echo "$1 : available"
    fi
}
 
if [ "$#" == "0" ]; then 
    echo "You need tu supply at least one argument!" 
    exit 1
fi 
 
# DOMAINS=( '.com' '.co.uk' '.net' '.info' '.mobi' '.org' '.tel' '.biz' '.tv' \
#     '.cc' '.eu' '.ru' '.in' '.it' '.sk' '.com.au' )
DOMAINS=( '.com' '.net' '.org' '.no' )
 
ELEMENTS=${#DOMAINS[@]} 
 
while (( "$#" ))
do
    if $( echo $1 | grep -q '\.' ) 
    then
        checkdomain $1
    else
        for (( i=0;i<$ELEMENTS;i++))
        do
            checkdomain $1${DOMAINS[${i}]}
#           whois $1${DOMAINS[${i}]} | egrep -q \
#               '^No match|^NOT FOUND|^Not fo|AVAILABLE|^No Data Fou|has not been regi|No entri'
#           if [ $? -eq 0 ]
#           then
#               echo "$1${DOMAINS[${i}]} : available"
#           fi
        done 
    fi
    shift 
done
