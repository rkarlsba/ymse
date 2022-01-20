#!/bin/bash
# vim:ts=4:sw=4:sts=4:et:ai

PATH=$PATH:/snap/bin:/usr/local/bin

# Variabler - se https://restic.readthedocs.io/en/latest/040_backup.html#environment-variables
RESTIC_USER="bruker"
RESTIC_SERVER="my.restic.server.com"
RESTIC_REPOSITORY="sftp:$RESTIC_USER@$RESTIC_SERVER:restic-repo"
RESTIC_PASSWORD_FILE="/usr/local/etc/restic-password.txt"
RESTIC_EXCLUDE_FILE="/usr/local/etc/restic-excludes.txt"
RESTIC_PRE_EXEC_JOB="/usr/local/bin/restic-backup-pre-exec.sh"
RESTIC_POST_EXEC_JOB="/usr/local/bin/restic-backup-post-exec.sh"
BACKUP_DIRS="/boot /"
RETCODE=0

trap exit_msg EXIT

function exit_msg {
    echo "-------------------------------------------------------------------------------"
    echo "* Backup av $( hostname ) avsluttet"
    if [ $RETCODE -ne 0 ]
    then
        echo "* Returkode $RETCODE"
    fi
    echo "* $( date "+%d. %B %Y klokka %H:%M:%S" )"
    echo "-------------------------------------------------------------------------------"
}

# Hovedprogram
echo "-------------------------------------------------------------------------------"
echo "* Starter backup av $( hostname )"
echo "* $( date "+%d. %B %Y klokka %H:%M:%S" )"
echo "-------------------------------------------------------------------------------"

if [ -x $RESTIC_PRE_EXEC_JOB ]
then
    echo "* Kjører pre-exec-job $RESTIC_PRE_EXEC_JOB"
    bash -c "$RESTIC_PRE_EXEC_JOB"
    RETCODE=$?
    if [ $RETCODE -ne 0 ]
    then
        echo "pre-exec-jobben returnerte $? - avslutter"
        exit 1
    fi
else
    echo "* Fant ikke passende pre-exec-job"
fi

# restic -r $RESTIC_REPOSITORY --password-file $RESTIC_PASSWORD_FILE --verbose backup /
# restic -r $RESTIC_REPOSITORY --password-file $RESTIC_PASSWORD_FILE --verbose backup --exclude-file $RESTIC_EXCLUDE_FILE /

# Eller med --one-file-system
restic -r $RESTIC_REPOSITORY \
    --password-file $RESTIC_PASSWORD_FILE \
    --verbose backup \
    --one-file-system \
    --exclude-file $RESTIC_EXCLUDE_FILE \
    /

if [ -x $RESTIC_POST_EXEC_JOB ]
then
    echo "* Kjører post-exec-job $RESTIC_POST_EXEC_JOB"
    bash -c "$RESTIC_POST_EXEC_JOB"
else
    echo "* Fant ikke passende post-exec-job"
fi

