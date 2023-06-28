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
BASENAME=$( basename $0 .sh )
TMPFILE=$( mktemp /tmp/$BASENAME.XXXXX )

OUTFILE='/var/run/zabbix/zabbix-needsreboot'
DEBBOOTFILE='/var/run/reboot-required'
DEBBOOTREASONFILE='/var/run/reboot-required.pkgs'
DEBUG=0
QUIET=0
OPT_LOCAL=0
OPT_CRON=0
STATUS='WARN'
NEEDSRESTARTING='needs-restarting -r'

# Set EMULATED to 1 to work on local data taken from an old machine
EMULATED=0
EMUSCRIPT="./emu-needs-restarting.sh -r"

function cleanup {
    rm -f $TMPFILE
}

trap cleanup EXIT

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
--emulated  Emulate \"needs-restarting -r\" on older machines
--clean     Remove named cache (usually for testing)
--help      This text\n"
    exit 0
}

linux_distro_check='zabbix_linux_distro_check.pl'

# Sanity check
OS=$( uname -s )
case $OS in
    Linux)
        linux_distro_check_cmd=$( which $linux_distro_check )
        if [ $? -ne 0 ]
        then
            echo "Can't find distro check tool '$linux_distro_check': $!"
            exit 2
        fi
        ;;
    *)
        echo "Sorry, operating system $OS isn't supported by this check"
        exit 3
        ;;
esac

DISTRO=$( $linux_distro_check_cmd )
DISTRO_VERS=$( $linux_distro_check_cmd --vers )
FULL_DISTRO_NAME=$( $linux_distro_check_cmd --friendly )

case $DISTRO in
    rhel|centos)
        # RHEL/Centos check
        # Just exit if running 6 or earlier
        MAJOR_VERS=$( echo $DISTRO_VERS | cut -d. -f1 )
        if [ $MAJOR_VERS -le 6 ]
        then
            echo "NO: Ignoring $FULL_DISTRO_NAME"
            exit 0
          fi


        while true
        do
            arg=$1
            echo "arg1 er $1"
            if [ "$arg" == "" ]
            then
                break
            fi
            if [ "$arg:0:2" = "--" ]
            then
                mode=${arg:2}
            fi
            case $arg in
                "--quiet")
                    QUIET=1
                    ;;
                "--emulated")
                    EMULATED=1
                    mode=${arg:2}
                    ;;
                "--clean")
                    rm -f $OUTFILE
                    exit 0
                    ;;
                "--direct"|"--local"|"--help")
                    # Just ignore these - mode is set above
                    ;;
                *)
                    echo "Unknown mode \"$arg\". Please see --help" >&2
                    exit 4
                    ;;
            esac
            echo mode is $mode
            shift
        done

        [ "$EMULATED" -gt 0 ] && NEEDSRESTARTING="$EMUSCRIPT"

        if [ "$mode" == "cron" -o "$mode" == "direct" ]
        then
            $NEEDSRESTARTING >/dev/null 2>&1 
            [ $? -eq 0 ] && needs_restart="NO" || needs_restart="YES"
            echo -n $needs_restart > $OUTFILE
            if [ $needs_restart == "YES" ]
            then
                EXIT=1
                echo ": " >> $OUTFILE
                count=0
                $NEEDSRESTARTING 2>/dev/null | while read line ; do echo "$line" >> $OUTFILE ;count=$(( $count + 1 )); done
                if [ $count -eq 0 ]
                then
                    echo '(for whatever reason (1))' >> $OUTFILE
                fi
            else
                EXIT=0
            fi
        fi
        if [ "$mode" == "direct" -o "$mode" == "local" ]
        then
            if [ -r $OUTFILE ]
            then
                [ $QUIET -gt 0 ] || cat $OUTFILE
            else
                [ $QUIET -gt 0 ] || echo $CHECKNAME ERROR
            fi
        fi
        ;;
    debian|ubuntu|raspbian)
        # Ignore arguments on debuntu - it's sufficiently fast without it
        if [ -f $DEBBOOTFILE ]
        then
            EXIT=1
            needs_restart="YES: "
            if [ -r $DEBBOOTREASONFILE -a ! -z $DEBBOOTREASONFILE ]
            then
                needs_restart+=$( printf "The following packages needs replenishing\n\r" )
                needs_restart+=$( cat "$DEBBOOTREASONFILE" )
            else
                needs_restart+='(for whatever reason (2))'
            fi
        else
            EXIT=0
            needs_restart="NO"
        fi
        [ $QUIET -gt 0 ] || echo "$needs_restart"
        ;;
    *)
        # Dunno
        echo "Unknown distro '$DISTRO'" >&2
        EXIT=1
        [ $DEBUG -gt 0 ] && echo "DEBUG[6]: Just set EXIT to $EXIT"
        ;;
esac

exit $EXIT

