#!/bin/bash
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker

for host in "$@"
do
    ipv4=$( host $host | awk '/has address/ { print $4 }' )
    ipv6=$( host $host | awk '/has IPv6 address/ { print $5 }' )
    echo $host har IPv4 $ipv4 og IPv6 $ipv6
    printf "IPv4 zabbix probe of $host:\t"; zabbix_get -s $ipv4 -k system.uptime
    printf "IPv6 zabbix probe of $host:\t"; zabbix_get -s $ipv6 -k system.uptime
done
