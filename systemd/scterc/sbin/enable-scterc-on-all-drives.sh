#!/bin/bash
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker:tw=80

# Set Error Recovery Control value to below values for read/write, respectively.
# If the drive doesn't support setting these values, set the SCSI timeout for
# that drive to 180 seconds, if this is enabled (by default, it's not)
#
# Written by Roy Sigurd Karlsbakk <roy@karlsbakk.net> and licensed under AGPLv3

ercval_read=70
ercval_write=70
scsi_timeout=180
verbose=1
happyhappyjoyjoy=1 # Ignore all errors
setscsitimeoutonerror=1
scsitimeout=120

#for d in $( lsblk -d | awk '{ if ($1 != "NAME") { print $1 } }' )
fail=""
for d in $( ls /dev/disk/by-id/ata* | grep -v -- -part )
do
    smartctl -l scterc,$ercval_read,$ercval_write $d > /dev/null
    if [ $? -ne 0 ]
    then
        if [ "$setscsitimeoutonerror" -gt 0 ]
        then
            dev=$( readlink -f $d | sed 's/\/dev//' )
            fail+="\t$d (setting scsi timeout to $scsitimeout on device)\n"
            echo $scsitimeout > /sys/block/$dev/device/timeout
        else
            fail+="\t$d\n"
        fi
    fi
done

if [ "$fail" != "" ]
then
    if [ "$verbose" -gt 0 ]
    then
        printf "The following drives failed to support SCTERC:\n$fail\n"
    fi
    if [ "$happyhappyjoyjoy" -gt 0 ]
    then
        exit 0
    fi
    exit 1
fi
exit 0
