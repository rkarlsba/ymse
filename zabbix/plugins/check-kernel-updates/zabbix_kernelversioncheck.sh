#!/bin/bash
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker:path+=/usr/local/bin

# Run with --cron or --local to avoid SELinux shite and similar.
# Run with --direct or no arguments to check directly.
# Copyleft Roy Sigurd Karlsbakk <roy@karlsbakk.net> <roysk@oslomet.no>
# Licensed under GPL v3

set -o errexit -o pipefail -o nounset

PATH=$PATH:/usr/local/bin

PROGNAME=$0
OUTFILE='/var/run/zabbix/zabbix-kernelversioncheck'
STATUS='WARNING'
RUNMODE='direct'
MESSAGE="No such error, this doesn't make sense!!!"
LONG_OPTS="cron,direct,local,help"
SHORT_OPTS=""  # none
DISTRO=$( zabbix_linux_distro_check.pl )
RETCODE=$?
runmode_count=0

if [ $RETCODE -ne 0 ]
then
    echo "Can't run distro check - exiting with $RETCODE"
    exit $RETCODE
fi

help() {
    cat <<EOT
Syntax: $PROGNAME [ --cron | --direct --local | --help ]
    --cron      Run as cronjob, saving data to intermediate file (good for
                slower systems like RHEL where yum/dnf is so slow a zabbix
                check can easily timeout.
    --direct    Just check, don't use intermediate file.
    --local     Read intermediate file, don't check directly.
    --help      This help.

Notes:
    - Use only one mode.
    - On Debian and related systems, the check is always done directly, since
      it only takes a few milliseconds.
EOT
    exit 0
}

# Optional: verify GNU getopt presence (on Linux it's usually fine)
# if ! getopt --test >/dev/null 2>&1; then
#     echo "Error: This script requires GNU getopt with long-option support." >&2
#     echo "Tip: On macOS, install GNU getopt (e.g., via Homebrew) and invoke that binary explicitly." >&2
#     exit 1
# fi

# Parse
PARSED="$(getopt -o "$SHORT_OPTS" -l "$LONG_OPTS" -- "$@")" || {
  MESSAGE="Failed to parse options"
  echo "$STATUS $MESSAGE" >&2
  exit 1
}
eval set -- "$PARSED"

while true
do
    case "$1" in
        --cron)
            RUNMODE="cron"
            runmode_count=$(( runmode_count+1 ))
            shift
            ;;
        --direct)
            RUNMODE="direct"
            runmode_count=$(( runmode_count+1 ))
            shift
            ;;
        --local)
            RUNMODE="local"
            runmode_count=$(( runmode_count+1 ))
            shift
            ;;
        --help)
            help
            exit 0
            ;;
        --)
            shift
            break
            ;;
        *)
            MESSAGE="Internal parsing error near '$1'"
            echo "$STATUS $MESSAGE" >&2
            exit 1
            ;;
    esac
done

# Enforce at most one runmode
if [ "$runmode_count" -gt 1 ]; then
  MESSAGE="Specify only one run mode (got $runmode_count)"
  echo "$STATUS $MESSAGE" >&2
  exit 1
fi

# No stray positional args allowed
if [ "$#" -gt 0 ]; then
  MESSAGE="Unexpected arguments: $*"
  echo "$STATUS $MESSAGE"
  exit 1
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
            echo "$STATUS $MESSAGE"
        fi
esac

