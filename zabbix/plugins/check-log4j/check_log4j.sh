#!/bin/bash
# vim:ts=4:sw=4:sts=4:et:ai

# See mktemp(1)
MKTMPTEMP="/tmp/log4jsearch.XXXXX"
REGEX="log4j\.jar|log4j-[0-9].*\.jar$|log4j-core.*\.jar$"
TMPOUTFILE=$( mktemp $MKTMPTEMP )
STATIC_STATUSFILE='/var/lib/zabbix/log4j.status'
OUTFILE=$TMPOUTFILE
JARLIST=$( mktemp $MKTMPTEMP )
QUIET=0
CRON=0
LOCAL=0
HITS=0
NORMAL=1
STATUS='OK'
STATUSTEXT=''
USE_LOCATE=0
DO_CHECK=1
UPDATE_STATUS_FILE=0
UPDATEDB=0
ME=$( basename $0 )
OS=$( uname )

# This is called on exit
function cleanup() {
    rm -f $TMPOUTFILE $JARLIST
}

function helpme() {
    echo "Syntax: $ME [--verbose | --cron | --local | --normal | --updatedb | --help ]

Flags:
  --verbose     Be verbose
  --cron        Don't run a normal check, but log it to local files, to be run by cron
  --local       Read results from an earlier check run with --cron
  --updatedb    Run updatedb before the check. To be used with --cron or normal check
  --normal      Run a normal check. This is the default.
  --help        This help.

Notes:
    - Either --cron, --local or --normal can be given, they cannot be combined.
      If neither is specified, --normal is assumed.
    - The flag --updatedb requires the script to be run as root.
    - Normal and cron checks should be run as root so that all files can be
      examined.
"

    exit 0
}

# Call this function on exit (except if we get SIGKILL, then we won't see it)
trap cleanup EXIT

# Iterate over commandline arguments
for arg in "$@"
do
    case "$arg" in
        --quiet)
            QUIET=1
            ;;
        --verbose)
            QUIET=0
            ;;
        --cron)
            UPDATE_STATUS_FILE=1
            CRON=1
            ;;
        --local)
            DO_CHECK=0
            LOCAL=1
            ;;
        --updatedb)
            UPDATEDB=1
            ;;
        --normal)
            NORMAL=1
            ;;
        --help)
            helpme
            ;;
        *)
            echo "Invalid argument: $arg" >&2
            ;;
    esac
done

# Sanity check

if [ $CRON -gt 0 ] && [ $LOCAL -gt 0 ]
then
    echo "ERROR: Can't use cron and local mode at the same time."
    exit 1
fi

# Don't run check if we're just doing a local run
if [ $DO_CHECK -gt 0 ]
then
    if [ $UID -gt 0 ]
    then
        STATUS="WARNING"
        STATUSTEXT="Running under a non-root user. This may not find all files. "
        if [ $UPDATEDB -gt 0 ]
        then
            STATUSTEXT+="--updatedb ignored "
            UPDATEDB=0
        fi
    fi

    # Check if we have locate
    if $( which locate > /dev/null 2>&1 )
    then
        # We need gnu locate, so we'll stick to checking if this is Linux or not
        if [ "$OS" == "Linux" ]
        then
            USE_LOCATE=1
        fi
    fi

    if [ $USE_LOCATE -gt 0 ]
    then
        if [ $UPDATEDB -gt 0 ]
        then
            updatedb
        fi
        locate -r "\.jar$" > $JARLIST
    else
        if [ $QUIET -eq 0 ]
        then
            if [ $UPDATEDB -gt 0 ]
            then
                echo "Ignoring --updatedb without locate" >&2
            fi
            if [ $CRON -eq 0 ]
            then
                echo "Warning: command 'locate' not found. Falling back to 'find', which may" >&2
                echo "time out Zabbix checks." >&2
            fi
        fi
        find / -iname '*.jar' -print > $JARLIST 2>/dev/null
    fi

    while read line
    do
        HIT=$( unzip -l "$line" 2>/dev/null | egrep -i "$REGEX" )
        if [ -n "$HIT" ]
        then
            HITS=$(( $HITS + 1 ))
            echo -e "$line\n$HIT\n" >> $OUTFILE
        fi
    done < $JARLIST
fi

if [ $LOCAL -gt 0 ]
then
    if [ $UPDATEDB -gt 0 ] && [ $QUIET -eq 0 ]
    then
        echo "--updatedb ignored on local checks" >&2
    fi
    if [ -r $STATIC_STATUSFILE ]
    then
        cat $STATIC_STATUSFILE
        exit 0
    else
        echo "WARNING: Can't read from sattus file $STATIC_STATUSFILE. Please run $0 --cron first."
        exit 1
    fi
fi

if [ $HITS -gt 0 ]
then
    if [ $CRON -gt 0 ]
    then
        echo "ERROR: Found a total of $HITS hits on log4j search:" > $STATIC_STATUSFILE
        cat $OUTFILE >> $STATIC_STATUSFILE
    else
        echo "ERROR: Found a total of $HITS hits on log4j search:"
        cat $OUTFILE
    fi
    exit 0
else
    STATUSTEXT+="log4j not found on this machine."
    if [ $CRON -gt 0 ]
    then
        echo "$STATUS: $STATUSTEXT" > $STATIC_STATUSFILE
    else
        echo "$STATUS: $STATUSTEXT"
    fi
fi

exit 0

