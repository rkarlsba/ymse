#!/bin/bash
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker

INSIGHTS="insights-client-boot.service insights-client-results.path insights-client.timer"
DISABLE=1

if [ "$#" -gt 0 ] && [ "$1" = "--enable" ]
then
    DISABLE=0
fi

for i in $INSIGHTS
do
    if [ "$DISABLE" -eq 0 ]
    then
        echo "enabling $i"
        systemctl unmask $i
        systemctl enable $i
    else
        systemctl disable $i
        systemctl mask $i
    fi
done
