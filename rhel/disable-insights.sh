#!/bin/bash
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker

INSIGHTS="insights-client-boot.service insights-client-results.path insights-client-results.service insights-client.service insights-client.timer"
for i in $INSIGHTS
do
    systemctl disable $i
    systemctl mask $i
done
