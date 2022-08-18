#!/bin/bash
# vim:ts=4:sw=4:si:ruler

# Simple locker example by Roy Sigurd Karlsbakk <roy@karlsbakk.net>

PROGNAME=simple-locker
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

# Insert code here
sleep 1000

# Clean up lock
rm -f $LOCKFILE
