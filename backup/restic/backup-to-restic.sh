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
# RESTIC_ONE_FILESYSTEM=0
# }}}

# Fail fast, catch errors in pipelines, undefined vars are errors
# -E ensures ERR trap is inherited in functions/subshells
set -Eeuo pipefail

RESTIC_CONFIG="/etc/default/restic"
RESTIC_OVERRIDE="/usr/local/etc/restic-override"

# Import system config
[ -f $RESTIC_CONFIG ] && source $RESTIC_CONFIG

# Import user config
[ -f $RESTIC_OVERRIDE ] && source $RESTIC_OVERRIDE

RESTIC_CMD="restic --verbose backup --exclude-file $RESTIC_EXCLUDE_FILE $RESTIC_BACKUP_DIRS"

# Sjekk om vi er på ei skrivebeskytta rot
if $( mount | grep -w /| grep -qw ro )
then
    export RESTIC_CACHE_DIR=/tmp/restic-cache
fi

# Trap specific signals (not EXIT)
# Trap on EXIT for the "finally" behavior, on ERR for error paths,
# and on common termination signals. SIGKILL/SIGSTOP cannot be trapped.
trap endgame EXIT
trap 'return 1' ERR
trap 'kill -TERM 0' HUP TERM
trap 'echo "Interrupted"; exit 130' INT

[ -d $RESTIC_LOG_DIR ] || mkdir -p $RESTIC_LOG_DIR || exit 1

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

# This should have been with getopts, but hell, this works too
for op in $@
do
    if [ "$op" = "--help" ]
    then
        echo "Syntax $0 [ -v | --one-filesystem | --no-pre-exec | --no-post-exec ]"
        exit 0
    elif [ "$op" = "-v" -o "$op" = "--verbose" ]
    then
        RESTIC_VERBOSE=1
        shift
    elif [ "$op" = "--one-file-system" -o "$op" = "--one-filesystem" -o "$op" = "-O" ]
    then
        RESTIC_ONE_FILESYSTEM=1
        shift
    elif [ "$op" = "--no-pre-exec" ]
    then
        RESTIC_PRE_EXEC_JOB=""
    elif [ "$op" = "--no-post-exec" ]
    then
        RESTIC_POST_EXEC_JOB=""
    fi
done

# Functions
verbose() {
    if [ $RESTIC_VERBOSE -gt 0 ]
    then
        echo $@
    fi
}

endgame() {
    # exit status of the script so far
    local exitcode=$? 

    set +e

    verbose "-------------------------------------------------------------------------------"
    verbose "Backup av $( hostname ) avsluttet"
    if [ $exitcode -ne 0 ]
    then
        verbose "Returkode $exitcode"
    fi
    verbose "$( date "+%d. %B %Y klokka %H:%M:%S" )"
    verbose "-------------------------------------------------------------------------------"

    # Best-effort: write a fresh snapshot list that Zabbix can ingest
    # (Works whether the backup succeeded or not, as long as the repo is reachable)
    # JSON output flag is global and supported by 'snapshots'.
    # Errors here should not hide the original status, so don't 'set -e' break this.
    restic snapshots --json >"$RESTIC_SNAPSHOT_JSON"

    rm -f $LOCKFILE

    exit $exitcode
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

if [ $RESTIC_VERBOSE -gt 0 ]
then
    RESTIC_VERBOSE_FLAG='--verbose'
else
    RESTIC_VERBOSE_FLAG='--quiet'
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
            if [ $RESTIC_ONE_FILESYSTEM -eq 0 ]
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

# Dette er trygt og enkelt
if ! restic snapshots >/dev/null 2>&1; then
    verbose "No repo found - running init"
    restic $RESTIC_VERBOSE_FLAG init
fi

# Hovedprogram
verbose "-------------------------------------------------------------------------------"
verbose "Starting backup of " . $( hostname )
verbose $( date "+%d. %B %Y at %H:%M:%S" )
verbose "-------------------------------------------------------------------------------"

if [ -x "$RESTIC_PRE_EXEC_JOB" ]
then
    verbose "Running pre-exec job $RESTIC_PRE_EXEC_JOB"
    bash -c "$RESTIC_PRE_EXEC_JOB"
    RESTIC_RETCODE=$?
    if [ $RESTIC_RETCODE -ne 0 ]
    then
        verbose "pre-exec job returned non-zero ($?) - stopping"
        exit 3
    fi
else
    verbose "Fant ikke passende pre-exec-job"
fi

restic $RESTIC_VERBOSE_FLAG backup $RESTIC_ONE_FILESYSTEM $RESTIC_EXCLUDE $RESTIC_BACKUP_DIRS

if [ -x "$RESTIC_POST_EXEC_JOB" ]
then
    verbose "Running post-exec job $RESTIC_POST_EXEC_JOB"
    bash -c "$RESTIC_POST_EXEC_JOB $RESTIC_VERBOSE"
else
    verbose "Could not find post-exec-job - ignoring it"
fi

# Clean up lock
rm -f $LOCKFILE
