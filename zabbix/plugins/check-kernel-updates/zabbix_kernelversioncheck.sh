#!/bin/bash
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker:path+=/usr/local/bin

# Run with --cron or --local to avoid SELinux shite and similar.
# Run with --direct or no arguments to check directly.
# Copyleft Roy Sigurd Karlsbakk <roy@karlsbakk.net> <roysk@oslomet.no>
# Licensed under GPL v3

PATH=$PATH:/usr/local/bin

OUTFILE='/var/run/zabbix/zabbix-kernelversioncheck'
STATUS='WARNING'

RUNMODE='direct'
MESSAGE=''

case "$#" in
    0)
        ;;
    1)
        case "$1" in
            "--cron")
                RUNMODE='cron'
                ;;
            "--direct")
                ;;
            "--local")
                RUNMODE='local'
                ;;
            *)
                MESSAGE="Illegal run mode '$1'"
                ;;
        esac
        ;;
    *)
        MESSAGE="Illegal number of arguments ($#)"
        echo $STATUS $MESSAGE
        exit 1
        ;;
esac

DISTRO=$( zabbix_linux_distro_check.pl )
RETCODE=$?

if [ $RETCODE -ne 0 ]
then
    echo "Can't run distro check - exiting with $RETCODE"
    exit $RETCODE
fi

case $DISTRO in
    rhel|centos)
        case $RUNMODE in
            cron|direct)
                LATEST=$( rpm -q --qf "%{VERSION}-%{RELEASE}.%{ARCH}\n" kernel | tail -n1 )
                ;;
            local)
                if [ -r $OUTFILE ]
                then
                    LATEST=$(head -n 1 $OUTFILE)
                else
                    MESSAGE="Cannot read outfile $OUTFILE"
                    echo $MESSAGE
                    exit 1
                fi
                ;;
        esac
        ;;
    debian|ubuntu)
        # FIXME Legg til 'latest' og sånt hvis det trengs - trur apparmor kan være snill nok til å tillate dette, da
        LATEST=$( dpkg -l linux-image* | awk '/^ii  linux-image-[0-9]/ { print $2 }' | sort -V | tail -1 | sed s/^linux-image-// )
        ;;
    *)
        MESSAGE="Dunno how to check kernel on distro $DISTRO"
        ;;
esac

case $RUNMODE in
    local)
        cat $OUTFILE
        ;;
    *)
        RUNNING=$(uname -r)
        if [ "$RUNNING" == "$LATEST" ]
        then
            STATUS="OK"
            MESSAGE="Latest kernel $RUNNING is running"
        else
            MESSAGE="Kernel $RUNNING is running, however $LATEST is installed. Server should be rebooted"
        fi

        if [ $RUNMODE == 'cron' ]
        then
            echo "$STATUS $MESSAGE" > $OUTFILE
        else
            echo "[1] $STATUS [2] $MESSAGE"
        fi
esac
