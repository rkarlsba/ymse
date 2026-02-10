#!/bin/bash
# vim:ts=4:sw=4:sts=4:et:ai:si:fdm=marker

set -e

function syntax {
    echo "Syntax: $0 [ -d /backup/dir ] [ -h max_hours ]" >&2
    exit 1
}

# Check if database backups are created/updated
MAX_HOURS=30
BACKUP_DIR='/var/backups/postgresql'
BACKUP_STATUS_FILE="$BACKUP_DIR/backup_success"

errors=0
errstr=''
now=$( date +%s )

backup_status_file_mtime=$( stat -c %Y "$BACKUP_STATUS_FILE" )
backup_status_file_age=$(( now - backup_status_file_mtime ))
backup_status_file_age_hours=$(( backup_status_file_age / 3600 ))
backup_status_file_age_days=$(( backup_status_file_age_hours / 24 ))

opts=$( getopt -o d:h: -l backupdir:,hours: -- "$@" )
eval set -- "$opts"

# Parse options
while true
do
    case "$1" in
        -d|--backupdir)
            BACKUP_DIR="$2"
            shift 2
            ;;
        -h|--hours)
            MAX_HOURS="$2"
            if ! [[ "$MAX_HOURS" =~ ^[0-9]+$ ]]
            then
                echo "$MAX_HOURS is not a number" >&2
                exit 2
            fi
            shift 2
            ;;
        --)
            shift
            break
            ;;
        *)
            echo "Intern feil: ukjent parameter: $1"
            exit 3
            ;;
    esac
done

MAX_MINUTES=$(( MAX_HOURS * 60 ))
MAX_SECONDS=$(( MAX_MINUTES * 60 ))

recent_backups=$( find $BACKUP_DIR -maxdepth 1 -mindepth 1 -type d -mmin -${MAX_MINUTES} | wc -l )

[ $? -eq 0 ] || syntax

if [ $? -ne 0 ]; then
    syntax
fi

if [ "$recent_backups" -lt 1 ]
then
    errstr+="Last backup is > $MAX_HOURS hours "
    errors=$(( errors + 1 ))
fi

if [ "$backup_status_file_age" -gt "$MAX_SECONDS" ]; then
    if [ $errors -gt 0 ]
    then
        errstr+="and "
    fi
    if [ "$backup_status_file_age_hours" -gt 72 ]
    then
        errstr+="$BACKUP_STATUS_FILE is older than $MAX_HOURS hours (${backup_status_file_age_days} days). "
    else
        errstr+="$BACKUP_STATUS_FILE is older than $MAX_HOURS hours (${backup_status_file_age_hours} hours). "
    fi
    errors=$(( errors + 1 ))
fi

if [ $errors -eq 0 ]
then
    echo "OK: PostgreSQL backup is updated within $MAX_HOURS hours"
else
    echo "CRITICAL: $errstr"
fi

