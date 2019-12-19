#!/bin/bash
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker:tw=80

# Set Error Recovery Control value to below values for read/write, respectively.
# If the drive doesn't support setting these values, set the SCSI timeout for
# that drive to 180 seconds, if this is enabled (by default, it's not)

ercval_read=70
ercval_write=70
scsi_timeout=180

for d in $( lsblk -d | awk '{ if ($1 != "NAME") { print $1 } }' )
do
    dev="/dev/$d"
    smartctl -l scterc,$ercval_read,$ercval_write $dev > /dev/null
#   smartctl -l scterc,$ercval_read,$ercval_write $dev || echo 180 > /sys/block/$dev/device/timeout
done

