#!/bin/bash
# vim:ts=4:sw=4:sts=4:et:ai

datetime=$( date +%F\ %T )
verbose=0
secs="$1"

if $( ! echo "$secs" | grep '^[0-9][0-9]*$' > /dev/null 2>&1 )
then
    echo "Syntax: $0 <secs>" >&2
    exit 1
fi

df1=$( df -k /mnt/media01/ | awk '/\/data/ { print $3 }' )
sleep $secs
df2=$( df -k /mnt/media01/ | awk '/\/data/ { print $3 }' )
transferred=$(( $df2 - $df1 ))
MBps=$(( $transferred / $secs / 1024 ))
MBp24h=$(( $MBps * 86400 ))
TBp24h=$(( $MBp24h / 1024 / 1024 ))
echo "[$datetime] $transferred kB overført i løpet av  $secs sekunder, dvs $MBps MB/s eller $TBp24h TB per døgn"
