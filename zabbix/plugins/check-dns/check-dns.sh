#!/bin/bash
# vim:ts=4:sw=4:sts=4:et:ai
#
# Written by Roy Sigurd Karlsbakk <roy@karlsbakk.net>
# Released under AGPLv3

max_timeout=5

syntax() {
    echo "Syntax: $0 <zone> <server>" >&2
    echo >&2
    echo "For instance $0 google.com 8.8.8.8" >&2
    exit 1
}

[ $# -eq 2 ] || syntax

zone=$1
server=$2

f_stdout=$( mktemp -q )
f_stderr=$( mktemp -q )
f_time=$( mktemp -q )

command timeout $max_timeout time -o $f_time host $zone $server > $f_stdout 2> $f_stderr
exitcode=$?

ok="good"

if [ $exitcode -ne 0 ]
then
    echo "ERROR: DNS lookup failed (zone '$zone' and server '$server')."
else
    echo -n "OK: "
    cat $f_time | perl -ne 'if (/(\d+\.\d+)user (\d+\.\d+)system (\d+\:\d+\.\d+)elapsed/) { print "Time spent in DNS request: User $1, System $2, Elapsed $3\n"; }'
fi

rm -f $f_stdout $f_stderr $f_time
