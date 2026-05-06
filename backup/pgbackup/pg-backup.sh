#!/bin/bash
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker:tw=100:wrap

# Make a backup if the given database(s) to file/dir.
# Syntax:
#   pg-dump.sh db1 [ db2 [ db3 [ ... ]]]

# Please exit on errors ;)
set -e

# Functions
show_help() {
    if [ "$1" != "" ]
    then
        printf "$1\n\n"
    fi
    echo "Syntax: $0 -d <output dir> [ -f <dumpformat ]"
    echo
    echo "EXAMPLES:"
    echo "$0 -d mybackup            Backup to dir 'mybackup'"
    echo "$0 -d NOW                 Backup to dir 'YYYY/MM/DD/HH/MM/SS'"
    echo "$0 -d TODAY               Backup to dir 'YYYY/MM/DD'"
    exit 0
}

# Date format, here as used in filename 'zabbix-2023-10-03_13_15_15.dump'
datefmt='%Y-%m-%d_%H-%M-%S'

# The actual date string output by date(1)
datestr=$( date +$datefmt )

# Parallel dump jobs - don't use more than you have cores, preferably less. Only
# supported by directory format. See pg_dump(1) for details.
dumpjobs=8

# Dump format is either plain (SQL), custom (compressed native postgres
# format), directory or tar. See pg_dump(1) for details.
dumpformat='custom'

# Output file, to be overridden
outdir=''

# No verbose by default
verbose=0
verbose_flag=''

# Pars args
#optstr=":f:d:hv:j"
optstr=":f:d:hvj:"

while getopts ${optstr} opt
do
    case ${opt} in
        h)
            show_help
            ;;
        f)
            dumpformat=${OPTARG}
            ;;
        d)
            outdir=${OPTARG}
            ;;
        v)
            verbose=$(( verbose + 1 ))
            verbose_flag+="-v "
            ;;
        j)
            dumpjobs=${OPTARG}
            ;;
        :)
            show_help "Option -${OPTARG} requires an argument."
            exit 1
            ;;
        ?)
            show_help "Invalid option: -${OPTARG}."
            exit 1
            ;;
    esac
done
shift $( expr $OPTIND - 1 )

# Sanity check
if [ "$outdir" = "" ]
then
    show_help "No outdir given, try again."
    exit 1
fi

case "$outdir" in
    "NOW")
        outdir=$( date +%Y/%m/%d/%H/%M/%S )
        ;;
    "TODAY")
        outdir=$( date +%Y/%m/%d )
        ;;
esac

# if [ ! -d "$outdir" ]
# then
#     mkdir -p "$outdir"
# fi

case $dumpformat in
    p|plain)
        ext='.sql'
        dumpjobs=1
        ;;
    c|custom)
        ext='.dump'
        dumpjobs=1
        ;;
    d|directory)
        ext=''
        ;;
    t|tar)
        ext='.tar'
        dumpjobs=1
        ;;
    *)
        echo "Dump format '$dumpformat' is unknown - giving up" >&2
        exit 1
        ;;
esac

dump_cmd=''
# Main code
for db in $@
do
    case "$dumpformat" in
        d|directory)
            if [ -d "$outdir" ]
            then
                echo "Output dir \"$outdir\" already exists"
                exit 1
            fi
            dump_cmd="pg_dump $verbose_flag --format=$dumpformat --jobs=$dumpjobs -f $outdir $db"
            ;;
        *)
            outfile=$outdir/$db-$datestr$ext
            if [ -f "$outfile" ]
            then
                echo "Output filename \"$outfile\" already exists"
                exit 2
            fi
            dump_cmd="pg_dump $verbose_flag --format=$dumpformat --jobs=$dumpjobs $db > $outfile"
            ;;
    esac

    if [ $verbose -gt 1 ]
    then
        echo "Running command \"$dumpcmd\""
    fi
    $dump_cmd
done

