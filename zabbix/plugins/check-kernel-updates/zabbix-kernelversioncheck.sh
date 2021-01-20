#!/bin/bash
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker:path+=/usr/local/bin

PATH=$PATH:/usr/local/bin

STATUS='WARNING'

DISTRO=$( zabbix_linux_distro_check.pl )
#DISTRO=$( /home/roysk/src/git/hioa/zabbix/linux-distro-check/zabbix_linux_distro_check.pl )
RETCODE=$?

if [ $RETCODE -ne 0 ]
then
    echo "Can't run distro check - exiting with $RETCODE"
    exit $RETCODE
fi

case $DISTRO in
    rhel|centos)
        LATEST=$( rpm -q --qf "%{VERSION}-%{RELEASE}.%{ARCH}\n" kernel | tail -n1 )
        ;;
    debian|ubuntu)
        LATEST=$( dpkg -l linux-image* | awk '/^ii  linux-image-[0-9]/ { print $2 }' | sort -V | tail -1 | sed s/^linux-image-// )
        MESSAGE="Dunno how to check kernel on distro $DISTRO"
        ;;
    *)
        MESSAGE="Dunno how to check kernel on distro $DISTRO"
        ;;
esac

RUNNING=$(uname -r)
if [ "$RUNNING" == "$LATEST" ]
then
    STATUS="OK"
    MESSAGE="Latest kernel $(uname -r) is running"
else
    MESSAGE="Kernel $RUNNING is running, however $LATEST is installed. Server should be rebooted!"
fi

echo $STATUS $MESSAGE
