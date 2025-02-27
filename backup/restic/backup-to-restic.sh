#!/bin/bash
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker

# Please see the README.md for documentation

# Variables - see https://restic.readthedocs.io/en/latest/040_backup.html#environment-variables {{{
# for details. These are basically the ones you need and is meant as a skeleton. Do not change
# these in this file, better set them in the restic config file, typically /etc/default/restic
# export RESTIC_REPOSITORY="sftp:restic:restic-repo"
# export RESTIC_PASSWORD_FILE="/usr/local/etc/restic-password.txt"
# 
# RESTIC_EXCLUDE_FILE="/usr/local/etc/restic-excludes.txt"
# RESTIC_EXCLUDE=""
# RESTIC_VERBOSE=""
# RESTIC_ONE_FILESYSTEM=""
# RESTIC_PRE_EXEC_JOB="/usr/local/bin/restic-backup-pre-exec.sh"
# RESTIC_POST_EXEC_JOB="/usr/local/bin/restic-backup-post-exec.sh"
# RESTIC_BACKUP_DIRS="/"
# ONE_FILESYSTEM=0
# RETCODE=0
# VERBOSE=0
# }}}

OS=$( uname -s )
RESTIC_CONFIG="/etc/default/restic"
PROGNAME=restic-backup
LOCKDIR=/var/lock
LOCKFILE=$LOCKDIR/$PROGNAME.lock
AUTOCLEANUP=1

# Trap specific signals (not EXIT)
trap "{ rm -f $LOCKFILE ; exit 1; }" SIGHUP SIGINT SIGQUIT SIGTERM

# Check if lockfile directory exits is writable and exit if not
if [ ! -d $LOCKDIR -o ! -w $LOCKDIR ]; then
	echo "Lockfile directory $LOCKDIR either doesn't exists or isn't writable by me - exiting" >&2
	exit 1;
fi

# Check if lockfile exists
if [ -f $LOCKFILE ]; then
	oldpid=`cat $LOCKFILE`

	# Check if old PID is numeric
	if `echo $oldpid | grep "^[0-9][0-9]*$" > /dev/null 2>&1`; then
		# Check if process is running
		if `kill -0 $oldpid > /dev/null 2>&1`; then
			echo "Old process running as PID $oldpid - exiting" >&2
			exit 1
		else
			echo "Old lockfile found, but no process running as pid $oldpid - removing lockfile" >&2
			rm -f $LOCKFILE
		fi
	else
		echo "Old lockfile found containing invalid PID - removing lockfile" >&2
		rm -f $LOCKFILE
	fi
fi

# Create lockfile
echo $$ > $LOCKFILE

# Import user config
source $RESTIC_CONFIG
exitcode=$?
if [ $exitcode -ne 0 ]
then
    echo "Can't opeen restic config file, \"$RESTIC_CONFIG\", giving up" >&2
    exit $exitcode
fi

# This should have been with getopts, but hell, this works too
for op in $@
do
    if [ "$op" = "--help" ]
    then
        echo "Syntax $0 [ -v | --one-filesystem | --no-pre-exec | --no-post-exec ]"
        exit 0
    elif [ "$op" = "-v" -o "$op" = "--verbose" ]
    then
        VERBOSE=1
        shift
    elif [ "$op" = "--one-file-system" -o "$op" = "--one-filesystem" -o "$op" = "-O" ]
    then
        ONE_FILESYSTEM=1
        shift
    elif [ "$op" = "--no-pre-exec" ]
    then
        RESTIC_PRE_EXEC_JOB=""
    elif [ "$op" = "--no-post-exec" ]
    then
        RESTIC_POST_EXEC_JOB=""
    fi
done

trap exit_msg EXIT

# Functions
function verbose {
    [ $VERBOSE -gt 0 ] || return
    echo $@
}

function exit_msg {
    verbose "-------------------------------------------------------------------------------"
    verbose "Backup av $( hostname ) avsluttet"
    if [ $RETCODE -ne 0 ]
    then
        verbose "Returkode $RETCODE"
    fi
    verbose "$( date "+%d. %B %Y klokka %H:%M:%S" )"
    verbose "-------------------------------------------------------------------------------"
}

# Sanity checks
restic_cmd=$( which restic )
exitcode=$?
if [ $exitcode -ne 0 ]
then
    echo "Missing restic command - please apt/yum/something install it first" >&2
    echo "and probably run a restic self-update as well" >&2
    exit $exitcode
fi

if [ ! -f "$RESTIC_PASSWORD_FILE" ]
then
    echo "ERROR: Need password file" >&2
    echo "       Create the file $RESTIC_PASSWORD_FILE and add a long and good" >&2
    echo "       password to it before chmodding it to 0600."
    exit 1
fi

case "$OS" in
    'Linux'|'Darwin'|'FreeBSD')
        mode=$( stat -c "%a" $RESTIC_PASSWORD_FILE )
        if [ "$mode" != "600" ]
        then
            echo "WARNING: Password file $RESTIC_PASSWORD_FILE has mode $mode, not 0600" >&2
            echo "         as recommended" >&2
        fi
        ;;
    *)
        echo "Unknown OS '$OS', not cheking restic password file mode" >&2
        ;;
esac

if [ $VERBOSE -gt 0 ]
then
    RESTIC_VERBOSE='--verbose'
else
    RESTIC_VERBOSE='--quiet'
fi

if [ $RESTIC_ONE_FILESYSTEM -gt 0 ]
then
    RESTIC_ONE_FILESYSTEM='--one-file-system'
fi

if [ -r "$RESTIC_EXCLUDE_FILE" ]
then
    RESTIC_EXCLUDE="--exclude-file $RESTIC_EXCLUDE_FILE"
else
    case $OS in
        Linux)
            if [ $ONE_FILESYSTEM -eq 0 ]
            then
                echo <<EOT
ERROR:
No exclude file found and run without --one-file-system. This will backup stuff
like /proc and /sys, which won't happen on my watch. See the docs on top of
this script for more info.
EOT
                exit 2
            fi
            ;;
        Darwin)
            ;;
    esac
fi

if $( restic snapshots 2>&1|grep '^Is there a repos' > /dev/null )
then
    verbose "No repo found - running init"
    restic $RESTIC_VERBOSE init
fi

# Hovedprogram
verbose "-------------------------------------------------------------------------------"
verbose "Starting backup of " . $( hostname )
verbose $( date "+%d. %B %Y at %H:%M:%S" )
verbose "-------------------------------------------------------------------------------"

if [ -x "$RESTIC_PRE_EXEC_JOB" ]
then
    export VERBOSE
    verbose "Running pre-exec job $RESTIC_PRE_EXEC_JOB"
    bash -c "$RESTIC_PRE_EXEC_JOB"
    RETCODE=$?
    if [ $RETCODE -ne 0 ]
    then
        verbose "pre-exec job returned non-zero ($?) - stopping"
        exit 3
    fi
else
    verbose "Fant ikke passende pre-exec-job"
fi

restic $RESTIC_VERBOSE backup $RESTIC_ONE_FILESYSTEM $RESTIC_EXCLUDE $RESTIC_BACKUP_DIRS

if [ -x "$RESTIC_POST_EXEC_JOB" ]
then
    verbose "Running post-exec job $RESTIC_POST_EXEC_JOB"
    bash -c "$RESTIC_POST_EXEC_JOB $RESTIC_VERBOSE"
else
    verbose "Could not find post-exec-job - ignoring it"
fi

# Clean up lock
rm -f $LOCKFILE
