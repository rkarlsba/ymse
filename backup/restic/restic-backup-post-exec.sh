#!/bin/bash

# Do something smart, cleaning up temporary files from restic-backup-pre-exec.sh
RESTIC_CONFIG='/etc/default/restic'

# Defaults - overridden in user config
export RESTIC_KEEP_DAILY=14
export RESTIC_KEEP_WEEKLY=5
export RESTIC_KEEP_MONTHLY=3
export RESTIC_KEEP_YEARLY=0

# Read config
source $RESTIC_CONFIG

# Clean up old backups
restic forget \
	--keep-daily $RESTIC_KEEP_DAILY \
	--keep-weekly $RESTIC_KEEP_WEEKLY \
	--keep-monthly $RESTIC_KEEP_MONTHLY \
	--keep-yearly $RESTIC_KEEP_YEARLY \
	--prune $@
