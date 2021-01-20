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
# Added EMULATED which works like the original, but runs another script,
# EMUSCRIPT to pick up the data, the script emulating the potential,
# alternating stderr/stdout output.
# - Emulation is only enabled on RHEL/CentOS, since Debian already deals with
# this on its own. It'll need to be tested on a debian machine, though.
# - I Should perhaps have had some getopt logic instead of all this
# hardcoding

# If server for some reason uses another locale, locally switch it to C for now
export LANG=C 
PATH=$PATH:/usr/local/bin

CHECKNAME='custom.needsreboot'
TMPFILE=$(mktemp /tmp/zabbix-checkupdates.XXXXX)
OUTFILE='/var/run/zabbix/zabbix-needsreboot'
DEBBOOTFILE='/var/run/reboot-required'
DEBUG=0
OPT_LOCAL=0
OPT_CRON=0
STATUS='WARN'
NEEDSRESTARTING='needs-restarting -r'

# Set EMULATED to 1 to work on local data taken from an old machine
EMULATED=0
EMUSCRIPT="./emu-needs-restarting.sh -r"
[ "$EMULATED" -gt 0 ] && NEEDSRESTARTING="$EMUSCRIPT"

function rootcheck {
    # Here, we need to be root
    if [ $EUID -ne 0 ]
    then
        echo "Need to be root to run this check - sorry (EUID is $EUID)" >&2
        exit 1
    fi
}

function printhelp {
    printf "Syntax:

zabbix-check-needs-reboot.sh [ --direct | --cron | --local | --clean | --help ]

--direct    Run directly, may take several seconds, but handy for testing
--cron      Run from cron to set local cache (because of above delay)
--local     Read the above cache and report
--clean     Remove named cache (usually for testing)
--help      This text\n"
    exit 0
}

DISTRO=$( zabbix_linux_distro_check.pl )

case $DISTRO in
    rhel|centos)
        # RHEL/Centos check
        if [ "$1" == "--direct" ]
        then
            mode="direct"
        elif [ "$1" == "--local" ]
        then
            mode="local"
        elif [ "$1" == "--cron" ]
        then
            mode="cron"
        elif [ "$1" == "--clean" ]
        then
            rm -f $OUTFILE
            exit 0
        elif [ "$1" == "--help" ]
        then
            printhelp
        else
            echo "Unknown mode \"$1\". Please see --help" >&2
            exit 3
        fi

        if [ "$mode" == "cron" -o "$mode" == "direct" ]
        then
            $NEEDSRESTARTING >/dev/null 2>&1 
            [ $? -eq 0 ] && needs_restart="NO" || needs_restart="YES"
            echo $needs_restart > $OUTFILE
            if [ $needs_restart == "YES" ]
            then
                $NEEDSRESTARTING 2>/dev/null | while read line ; do echo "$line" >> $OUTFILE ; done 
            fi
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
    debian|ubuntu|raspbian)
        # Ignore arguments on debuntu - it's sufficiently fast without it
        if [ -f $DEBBOOTFILE ]
        then
            needs_restart="YES: "
            if [ -r $DEBBOOTFILE -a ! -z $DEBBOOTFILE ]
            then
                needs_restart+=$( cat $DEBBOOTFILE )
            else
                needs_restart+="(for whatever reason)"
            fi
        else
            needs_restart="NO"
        fi
        echo $needs_restart
        ;;
    *)
        # Dunno
        echo "Unknown distro '$DISTRO'"
        EXIT=1
        [ $DEBUG -gt 0 ] && echo "DEBUG[6]: Just set EXIT to $EXIT"
        ;;
esac

exit $EXIT

