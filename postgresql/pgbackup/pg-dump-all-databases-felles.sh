#!/bin/bash
# vim:ts=4:sw=4:ai:isfname-==:fdm=marker

verbose=0
rotate=10
dst="$HOME/backups/felles" # /var/lib/postgresql/backups
pguser='postgres'
export LC_ALL=C

[ -d $dst ] || mkdir -p $dst

# Not in use - use logrotate(8) instead {{{

# rotate {
# 	exit 1
# 	filename = shift
# 	[ -f "$filename" ] || exit 0
# 	[ -f "$filename.$rotate" ] && echo rm -f $filename.$rotate
# 	for (( ext=$( $rotate - 1 ); ext>=1; x-- ))
# 	do
# 		newext=$( $ext - 1 )
# 		echo mv $filename
# 	done
# }
# }}}
# Formats {{{
#          p
#          plain
#              Output a plain-text SQL script file (the default).
#
#          c
#          custom
#              Output a custom-format archive suitable for input into pg_restore. Together with the directory output format, this is the most flexible output format in that it allows manual selection and reordering
#              of archived items during restore. This format is also compressed by default.
#
#          d
#          directory
#              Output a directory-format archive suitable for input into pg_restore. This will create a directory with one file for each table and blob being dumped, plus a so-called Table of Contents file
#              describing the dumped objects in a machine-readable format that pg_restore can read. A directory format archive can be manipulated with standard Unix tools; for example, files in an uncompressed
#              archive can be compressed with the gzip tool. This format is compressed by default and also supports parallel dumps.
#
#          t
#          tar
#              Output a tar-format archive suitable for input into pg_restore. The tar format is compatible with the directory format: extracting a tar-format archive produces a valid directory-format archive.
#              However, the tar format does not support compression. Also, when using tar format the relative order of table data items cannot be changed during restore.
# }}}

if [ "$1" = "-f" ]
then
	shift
	fmt=$1
	shift
	case $fmt in
		'c'|'custom')
			fmt_arg='--format=custom'
			;;
		'p'|'plain')
			fmt_arg='--format=plain'
			;;
		'd'|'directory')
			fmt_arg='--format=directory'
			;;
		't'|'tar')
			fmt_arg='--format=tar'
			;;
		*)
			echo "Given format '$fmt' is unknown - exiting" >&2
			exit 1
			;;
	esac
fi
if [ "$1" = "-d" ]
then
	shift
	dst=$1
	shift
fi
if [ "$1" = "-v" ]
then
	shift
	verbose=1
fi
if [ "$1" = "-f" ]
then
	shift
	force=1
fi
if [ "$1" = "-r" ]
then
	shift
	rotate=$1
	shift
fi
if [ $rotate -gt 0 ]
then
	echo "WARN: Backup rotation not implemented" >&2
fi

if [ "$LOGNAME" != "$pguser" ]; then
    echo "This thing can't be run as \"$USER\", only \"$pguser\"" >&2
    exit 1
fi

databases=`echo '\\l' | psql | egrep -v '^\(|^-|^\s*Name.*Owner|List of databases'  | \
	awk '{ print $1 }'| egrep -v '^$|^\||template[01]'`

# dump databases
for db in $databases
do
    dst_file="$dst/$db.dump"
	[ $verbose -gt 0 ] && echo -n "Dumping database $db to $dst_file..."
	pg_dump $fmt_arg $db > $dst_file || exit 1
	[ $verbose -gt 0 ] && echo
done
