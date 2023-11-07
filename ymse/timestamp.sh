#!/bin/bash
# vim:ts=4:sw=4:sts=4:et:ai

avoid_colon=0

# man test
if [ "$#" -eq 1 ]
then
    separator=$1
    avoid_colon=1
fi

# See strftime(3) for the options

# %F is equivalent to “%Y-%m-%d”.
datestr=$( date +%F )

if [ "$avoid_colon" -gt 0 ]
then
    # %H is replaced by the hour (24-hour clock) as a decimal number (00-23).
    # %M is replaced by the minute as a decimal number (00-59).
    # %S is replaced by the second as a decimal number (00-60).
    timestr=$( date +%H${separator}%M${separator}%S )
else
    # %T is equivalent to “%H:%M:%S”.
    timestr=$( date +%T )
fi

echo ${datestr}_${timestr}
