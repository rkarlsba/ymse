#!/bin/bash
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker:tw=80:wrap

# Make a backup if the given database(s) to file/dir.
# Syntax:
#   pg-dump.sh db1 [ db2 [ db3 [ ... ]]]

# Please exit on errors ;)
set -e

# Date format, here as used in filename 'zabbix-2023-10-03_13_15_15.dump'
datefmt='%Y-%m-%d_%H_%M_%S'

# The actual date string output by date(1)
datestr=$( date +$datefmt )

# Parallel dump jobs - don't use more than you have cores, preferably less. Only
# supported by directory format. See pg_dump(1) for details.
dumpjobs=8

# Dump format is either plain (SQL), custom (compressed native postgres
# format), directory or tar. See pg_dump(1) for details.
dumpformat='custom'

# Pars args
optstr=":f:"

while getopts ${optstr} opt
do
    case ${opt} in
        f)
            dumpformat=${OPTARG}
            ;;
        :)
            echo "Option -${OPTARG} requires an argument."
            exit 1
            ;;
        ?)
            echo "Invalid option: -${OPTARG}."
            exit 1
            ;;
    esac
done
shift $( expr $OPTIND - 1 )

# Sanity check
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

# Main code
for db in $@
do
    filename=$db-$datestr$ext
    pg_dump --format=$dumpformat --jobs=$dumpjobs $db > $filename
done
