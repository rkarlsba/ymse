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

# If --local is given, run a local check, that
if [ "$1" == "--local" ]
then
    [ $DEBUG -gt 0 ] && echo "DEBUG[1]: Setting OPT_LOCAL is to 1, \$1 == '$1'"
    OPT_LOCAL=1
elif [ "$1" == '--cron' ]
then
    [ $DEBUG -gt 0 ] && echo "DEBUG[2]: Setting OPT_CRON is to 1, \$1 == '$1'"
    OPT_CRON=1
fi

DISTRO=$( zabbix_linux_distro_check.pl )

case $DISTRO in
    rhel|centos)
        # RHEL/Centos check
        if $( which yum > /dev/null 2>&1 )
        then
            if [ $OPT_LOCAL -gt 0 ]
            then
                cat $OUTFILE > $TMPFILE
            else
                yum check-update > $TMPFILE 2>&1
            fi
            EXIT="$?"
            [ $DEBUG -gt 0 ] && echo "DEBUG[3]: Just set EXIT to $EXIT"

            if $( egrep -q -i '^(Error|Cannot)' $TMPFILE )
            then
                STATUS="FAIL"
            elif [ "$EXIT" = "0" ]
            then
                STATUS='OK'
            fi
        fi
        if [ $OPT_CRON -gt 0 ]
        then
            [ $DEBUG -gt 0 ] && echo "DEBUG[7]: OPT_LOCAL is $OPT_LOCAL"
            echo $CHECKNAME $STATUS > $OUTFILE
            cat $TMPFILE > $OUTFILE
        else
            [ $DEBUG -gt 0 ] && echo "DEBUG[8]: OPT_LOCAL is $OPT_LOCAL"
            echo $CHECKNAME $STATUS
            cat $TMPFILE
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

