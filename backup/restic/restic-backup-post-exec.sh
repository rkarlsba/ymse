#!/bin/bash

# Do something smart, cleaning up temporary files from restic-backup-pre-exec.sh
RESTIC_CONFIG='/etc/default/restic'

# Read config
source $RESTIC_CONFIG

# Clean up old backups
restic forget --keep-daily 14 --keep-weekly 5 --keep-monthly 3 --keep-yearly 0 --prune
