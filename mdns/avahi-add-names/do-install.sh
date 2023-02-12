#!/bin/bash
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker

SERVICE="avahi-add-names.service"

SRC_SCRIPT="bin/avahi-add-names.sh"
SRC_CONFIG="conf/avahi-add-names"
SRC_SERVICE="systemd/$SERVICE"

DST_SCRIPT="/usr/local/sbin/avahi-add-names.sh"
DST_CONFIG="/etc/default/avahi-add-names"
DST_SERVICE="/etc/systemd/system/$SERVICE"

if [ -f $DST_SCRIPT ]
then
    mv -f $DST_SCRIPT $DST_SCRIPT.bk
fi
cp -f $SRC_SCRIPT $DST_SCRIPT

if [ -f $DST_CONFIG ]
then
    cp $SRC_CONFIG $DST_CONFIG.new
else
    cp $SRC_CONFIG $DST_CONFIG
fi

cp -f $SRC_SERVICE $DST_SERVICE
systemctl enable $SERVICE
systemctl start $SERVICE

echo "All hopefully installed"
