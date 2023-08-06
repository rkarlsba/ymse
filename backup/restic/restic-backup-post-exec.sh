#!/bin/bash

# Do something smart, cleaning up temporary files from restic-backup-pre-exec.sh
RESTIC_CONFIG='/etc/default/restic'

# Read config
source $RESTIC_CONFIG

# Clean up old backups
restic forget --keep-daily $KEEP_DAILY --keep-weekly $KEEP_WEEKLY --keep-monthly $KEEP_MONTHLY --keep-yearly $KEEP_YEARLY --prune $@
