#!/bin/bash
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker

CONFIG='/etc/default/avahi-extra-names'

if [ ! -r $CONFIG ]
then
    printf "Can't read config file '$CONFIG', exiting\n" >&2
    exit 1
fi
source $CONFIG

# EXTRA_MDNS_NAMES='lagus.local trede.local'
if [ -z "$EXTRA_MDNS_NAMES" ]
then
    printf "Variable EXTRA_MDNS_NAMES not set, check config file $CONFIG\n" >&2
    exit 2
fi

# EXTRA_MDNS_NIC='wlan0'
if [ -z "$EXTRA_MDNS_NIC" ]
then
    printf "Variable EXTRA_MDNS_NIC not set, check config file $CONFIG\n" >&2
    exit 3
fi

ip=$( ip -4 -br addr list dev $EXTRA_MDNS_NIC | awk '{ print $3 }' | cut -d/ -f1 )

for name in $EXTRA_MDNS_NAMES
do
    if [ "$EXTRA_MDNS_NAMES_DOIT" -gt 0 ]
    then
        /usr/bin/avahi-publish -a -R $name $ip &
    else
        echo "/usr/bin/avahi-publish -a -R $name $ip &"
    fi
done
