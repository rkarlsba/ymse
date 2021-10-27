#!/bin/bash
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker:tw=80

# License and docs {{{
#
# Written by Roy Sigurd Karlsbakk <roy@karlsbakk.net>
# Licensed under AGPL v3. See https://www.gnu.org/licenses/agpl-3.0.en.html for
# details.
#
# This script attempts to retrieve a client count from a Squid proxy running its
# internal snmp server. This normally listens on port 3401 (not 161), but can be
# configured otherwise (but not 161 or anything < 1024 unless you run squid as
# root and in that case, you're obviously doing something utterly wrong).
#
# }}}

COMMUNITY="public"
PORT=3401
HOST=""
DEBUG=0

function error {
    echo -n "Usage: "
    echo $1
    echo "$0 [ -c community ] [ -p port ] hostname/ip"
    exit 1
}

while getopts ":c:p:hd" arg
do
    case $arg in
        c)
            COMMUNITY=${OPTARG}
            ;;
        d)
            DEBUG=$(( $DEBUG + 1 ))
            ;;
        p)
            PORT=${OPTARG}
            ;;
        h | *)
            error
            exit 0
            ;;
    esac
done

HOST=$1

if [ "$HOST" = "" ]
then
    error "Need hostname"
fi

[ $DEBUG -gt 0 ] && echo "snmpwalk -c $COMMUNITY -v 1 $HOST:$PORT 1.3.6.1.4.1.3495.1.5.2.2.2 2>/dev/null|grep -v '^End of MIB'|wc -l"
snmpwalk -c $COMMUNITY -v 1 $HOST:$PORT 1.3.6.1.4.1.3495.1.5.2.2.2  2>/dev/null|grep -v '^End of MIB'|wc -l
