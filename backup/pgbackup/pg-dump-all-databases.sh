#!/bin/bash
# vim:ts=4:sw=4:ai:isfname-==:fdm=marker

# Written by Roy Sigurd Karlsbakk <roy@karlsbakk.net>
# Released under BSD-3-Clause-Attribution - See LICENSE for details.

verbose=0
verbarg=''

if [ "$1" = "-v" ]
then
    verbose=1
	verbarg='-v'
fi

export LC_ALL=C

pguser='postgres'

if [ "$LOGNAME" != "$pguser" ]; then
    echo "This thing can't be run as \"$LOGUSER\", only \"$pguser\"" >&2
    exit 1
fi

databases=`echo '\\l' | psql | egrep -v '^\(|^-|^\s*Name.*Owner|List of databases'  \
    | awk '{ print $1 }'| egrep -v '^$|^\||template[01]'`
dst="$HOME/backups" # Usually /var/lib/postgresql/backups

[ -d $dst ] || mkdir -p $dst

# dump databases
for db in $databases
do
    dst_file="$dst/$db.dump"
    [ $verbose -gt 0 ] && echo -n "Dumping database $db to $dst_file..."
    pg-dump -f $dumpformat --format=custom -c $compression $db > $dst_file || exit 1
    [ $verbose -gt 0 ] && echo
done

./dump_globals.sh
