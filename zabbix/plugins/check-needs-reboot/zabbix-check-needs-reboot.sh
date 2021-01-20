#!/bin/bash
# vim:ts=4:sw=4:et:ai:fdm=marker:bg=dark

# Written by Roy Sigurd Karlsbakk <roysk@oslomet.no> or <roy@karlsbakk.net> for
# use with Zabbix. This uses zabbix_linux_distro_check.pl found in this git
# repo to lookup linux distro details. Written using zabbix-checkupdates.sh,
# also in this git repo, as a basepoint
#
# The reason for this check (along with its brother, zabbix-checkupdates.sh), is
# to allow for checks of yum-related stuff from zabbix. Since zabbix usually
# times out after 10 seconds, a fresh yum cache will usually take longer than
# this to fill up and the yum job will be interrupted. This may also be the case
# with increased timeout. This job just batches the check in cron (run script
# with --cron) and then reads this file to check the result when running the
# script with the --local argument.
#
# The script can be run with the --direct argument to check directly or with the
# --clean argument to remove the cache file. All checks will leave a cache file.

# 2021-01-20
# New script (nice)

# If server for some reason uses another locale, locally switch it to C for now
export LANG=C 
PATH=$PATH:/usr/local/bin

TMPFILE=$(mktemp /tmp/zabbix-needsreboot.XXXXX)
CHECKNAME='custom.needsreboot'
OUTFILE='/var/run/zabbix/zabbix-needsreboot'
DEBUG=0
OPT_LOCAL=0
OPT_CRON=0
STATUS='WARN'

function rootcheck {
    # Here, we need to be root
    if [ $EUID -ne 0 ]
    then
        echo "Need to be root to run this check - sorry (EUID is $EUID)" >&2
        exit 1
    fi
}

if [ "$1" == "--direct" ]
then
    mode="direct"
elif [ "$1" == "--local" ]
then
    mode="local"
elif [ "$1" == '--cron' ]
then
    mode="cron"
elif [ "$1" == '--clean' ]
then
    rm -f $OUTFILE
    exit 0
else
    echo "Unknown mode \"$1\"" >&2
    exit 3
fi

DISTRO=$( zabbix_linux_distro_check.pl )

case $DISTRO in
    rhel|centos)
        # RHEL/Centos check

        if [ "$mode" == "cron" -o "$mode" == "direct" ]
        then
            needs-restarting > /dev/null 2>&1
            [ $? -eq 0 ] && needs_restart="NO" || needs_restart="YES"
            echo $CHECKNAME $needs_restart > $OUTFILE
        fi
        if [ $mode == "direct" -o "$mode" == "local" ]
        then
            if [ -r $OUTFILE ]
            then
                cat $OUTFILE
            else
                echo $CHECKNAME ERROR
            fi
        fi
        ;;
    debian|ubuntu)
        # Ignore --local and --cron on debuntu - it's fast
        APT_SEC=$( apt-get -s upgrade | grep -ci ^inst.*security | tr -d '\n' )
        APT_UPD=$( apt-get -s upgrade | grep -iPc '^Inst((?!security).)*$' | tr -d '\n' )
        if [ $APT_SEC -gt 0 -o $APT_UPD -gt 0 ]
        then
            apt-get --just-print upgrade > $TMPFILE
            STATUS='WARNING'
            EXIT=1
            [ $DEBUG -gt 0 ] && echo "DEBUG[4]: Just set EXIT to $EXIT"
        else
            STATUS='OK'
            EXIT=0
            [ $DEBUG -gt 0 ] && echo "DEBUG[5]: Just set EXIT to $EXIT"
        fi
        echo $CHECKNAME $STATUS
        ;;
    *)
        # Dunno
        echo "Unknown distro '$DISTRO'"
        EXIT=1
        [ $DEBUG -gt 0 ] && echo "DEBUG[6]: Just set EXIT to $EXIT"
        ;;
esac

rm -f $TMPFILE

exit $EXIT

