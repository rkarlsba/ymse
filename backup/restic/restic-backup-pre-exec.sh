#!/bin/bash
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker

# Set config file path
RESTIC_CONFIG='/etc/default/restic'

# Read config
source $RESTIC_CONFIG

if [ "$VERBOSE" -gt 0 ]
then
    VERBFLAG="-v"
else
    VERBFLAG="-v"
fi

# Do something smart, like dumping databases etc

