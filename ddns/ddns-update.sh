#!/bin/bash
# vim:isfname-==:ts=4:sw=4:sts=4:et:ai

# Simple rfc2136 DIY dyndns
#
# You'll need ddns setup in the zone on the DNS server and a key configured
# there for the client (this one).
#
# Written by Roy Sigurd Karlsbakk <roy@karlsbakk.net> and licensed under
# GPLv3. See the LICENSE file for details.

# Variables
KEY="/path/to/ddns-key.my.domain.key"
NS="ns.my.domain"
VM="vm1 vm2 vm3"
VMN=$( echo $VM | sed s/-vm//g )
ZONE="my.domain."
DOMAIN=$( echo $ZONE | sed s/\.$// )
HOSTS="$ZONE "
TTL=900
IPv4_FILE='/var/lib/ddns/ddns4.ip'
FORCE=0

# Update without having a new IP?
if [ "$1" = "--force" ]
then
    FORCE=1
fi

# Fetch IPv4 address
MYIPv4=$( curl -s https://ip4.karlsbakk.net/ip.php )

# Check if this looks like an IP address
if [[ $MYIPv4 =~ ^[0-9]+\.[0-9]+\.[0-9]+\.[0-9]+$ ]]
then
    logger -p local0.debug "[ddns] $MYIPv4 looks like a good IP address"
else
    logger -p local0.notice "[ddns] $MYIPv4 does _not_ look like a good IP address. Exiting"
    exit 1
fi

# Compare old and new IP if the old one exists
IP_FILE_DIR=$( dirname $IPv4_FILE )
if [ ! -d $IP_FILE_DIR ]
then
    mkdir -p $IP_FILE_DIR
    if [ $? -ne 0 ]
    then
        echo "Something didn't work out: $!"
        exit 1
    fi
fi

# Check if we have a new IP address
if [ -f "$IPv4_FILE" ]
then
    OLDIPv4=$( cat $IPv4_FILE )
    if [ "$OLDIPv4" = "$MYIPv4" ]
    then
        if [ "$FORCE" -gt 0 ]
        then
            # så fortsetter vi
            logger -p local0.info "[ddns] Official IPv4 still on $MYIPv4, but --force given, going on"
        else
            # ellers avslutter vi
            logger -p local0.info "[ddns] Official IPv4 still on $MYIPv4 - exiting"
            exit 0
        fi
    else
        logger -p local0.notice "[ddns] Official IPv4 changed from $OLDIPv4 to $MYIPv4 - updating DNS"
    fi
else
    logger -p local0.notice "[ddns] Can't find old IPv4 - setting $MYIPv4"
fi

# Update IPv4 file
echo $MYIPv4 > $IPv4_FILE

# Make a list with hosts to be updated
for h in $VM $VMN
do
    HOSTS+="$h.$DOMAIN "
done

# Update DNS with new data
for HOST in $HOSTS
do
    nsupdate -k $KEY -v << EOF
server $NS
zone $ZONE
update delete $HOST A
update add $HOST $TTL A $MYIPv4
send
EOF
done
