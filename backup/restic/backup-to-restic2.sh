#!/bin/bash
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker

# Please see the README.md for documentation

# Documentation {{{
#
# Set ssh user, hostname and port in $HOME/.ssh/config - it should look
# something like this (without the initial #). See man ssh_config(5) for more
# details.
# 
# host restic
# 	Hostname restic.my.tld
##	AddressFamily inet
##	Port 3422
# 	User mysshusername
#
# The exclude file referred to below must exist if you're not using
# --one-file-system. It should include at least these four lines (without #)
#
# /proc
# /sys
# /dev
# /run
#
# }}}

# Fail fast, catch errors in pipelines, undefined vars are errors
# -E ensures ERR trap is inherited in functions/subshells
set -Eeuo pipefail

PATH=/bin:/usr/bin:/usr/local/bin:/sbin:/usr/sbin:/usr/local/sbin:/snap/bin

# Variabler - se https://restic.readthedocs.io/en/latest/040_backup.html#environment-variables
export RESTIC_REPOSITORY="sftp:restic:restic-repo"
export RESTIC_PASSWORD_FILE="/usr/local/etc/restic-password.txt"

RESTIC_LOG_DIR="/var/log/restic"
RESTIC_SNAPSHOT_JSON="$RESTIC_LOG_DIR/snapshots.json"
# RESTIC_LATEST_ID_TXT="$RESTIC_LOG_DIR/latest_snapshot.txt"
# RESTIC_BACKUP_LOG="$RESTIC_LOG_DIR/backup.jsonl"

RESTIC_EXCLUDE_FILE="/usr/local/etc/restic-excludes.txt"
RESTIC_EXCLUDE=""
RESTIC_VERBOSE=""
RESTIC_ONE_FILESYSTEM=""
RESTIC_PRE_EXEC_JOB="/usr/local/bin/restic-backup-pre-exec.sh"
RESTIC_POST_EXEC_JOB="/usr/local/bin/restic-backup-post-exec.sh"
RESTIC_CMD="restic --verbose backup --exclude-file $RESTIC_EXCLUDE_FILE /"
RESTIC_BACKUP_DIRS="/"
ONE_FILESYSTEM=0
RETCODE=0
VERBOSE=0
OS=$( uname -s )

# Import user config
source /etc/default/restic

# burde vært getopts
for op in $@
do
    if [ "$op"="-v" ]
    then
        VERBOSE=1
        shift
    elif [ "$op"="--one-file-system" -o "$op"="--one-filesystem" -o "$op"="-O" ]
    then
        ONE_FILESYSTEM=1
        shift
    fi
done

trap exit_msg EXIT

endgame() {
    # exit status of the script so far
    local exitcode=$? 

    # Best-effort: write a fresh snapshot list that Zabbix can ingest
    # (Works whether the backup succeeded or not, as long as the repo is reachable)
    # JSON output flag is global and supported by 'snapshots'.
    # Errors here should not hide the original status, so don't 'set -e' break this.
    set +e
    restic snapshots --json >"$RESTIC_SNAPSHOT_JSON"

    exit $exitcode
}

verbose() {
    if [ $VERBOSE -gt 0 ]
    then
        echo $@
    fi
}

exit_msg() {
    verbose "-------------------------------------------------------------------------------"
    verbose "Backup av $( hostname ) avsluttet"
    if [ $RETCODE -ne 0 ]
    then
        verbose "Returkode $RETCODE"
    fi
    verbose "$( date "+%d. %B %Y klokka %H:%M:%S" )"
    verbose "-------------------------------------------------------------------------------"
}

# Trap on EXIT for the "finally" behavior, on ERR for error paths,
# and on common termination signals. SIGKILL/SIGSTOP cannot be trapped.
trap endgame EXIT
trap 'return 1' ERR
trap 'kill -TERM 0' HUP TERM
trap 'echo "Interrupted"; exit 130' INT

[ -d $RESTIC_LOG_DIR ] || mkdir -p $RESTIC_LOG_DIR || exit 1

# Sjekker
if [ ! -f "$RESTIC_PASSWORD_FILE" ]
then
    echo "ERROR: Need password file" >&2
    echo "       Create the file $RESTIC_PASSWORD_FILE and add a long and good" >&2
    echo "       password to it before chmodding it to 0600."
    exit 1
fi

if [ "$OS" = "Linux" ] 
then
    mode=$( stat -c "%a" $RESTIC_PASSWORD_FILE )
    if [ "$mode" != "600" ]
    then
        echo "WARNING: Password file $RESTIC_PASSWORD_FILE has mode $mode, not 0600"
        echo "         as recommended"
    fi
fi

if [ $VERBOSE -gt 0 ]
then
    RESTIC_VERBOSE='--verbose'
else
    RESTIC_VERBOSE='--quiet'
fi

if [ $ONE_FILESYSTEM -gt 0 ]
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

if [ -x $RESTIC_PRE_EXEC_JOB ]
then
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

if [ -x $RESTIC_POST_EXEC_JOB ]
then
    verbose "Running post-exec job $RESTIC_POST_EXEC_JOB"
    bash -c "$RESTIC_POST_EXEC_JOB"
else
    verbose "Could not find post-exec-job - ignoring it"
fi

