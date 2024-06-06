#!/bin/bash
# vim:ts=4:sw=4:sts=4:et:ai

databases=$( echo show databases | mysql | egrep -v "^Database|^lost+found|^information_schema|^performance_schema" )
backuptarget="/var/backups/mariadb"
mxdbdump="mariadb-dump" # or "mysqldump" or whatever comes next
dbbbfname="%s-$( date "+%Y-%m-%d" ).sql"
comp='cat'
ext=''
force=0

function show_help {
    [ $# -gt 0 ] && echo $@
    echo <<EOT
Syntax: $0 [ -b | -g | -x ] [ -h ] [ -f ]

Having
    -b      == Compress backups with bzip2
    -g      == Compress backups with gzip
    -x      == Compress backups with xz

    -h / -? == Show this help

    -f      == Forcibly overwrite old backup files with the same name as new ones
EOT
    exit 0
}

# Parse arguments first
optstr=":bfghx?"

while getopts ${optstr} opt
do
    case ${opt} in
        'b')
            comp='bzip2 -c'
            ext='.bz2'
            ;;
        'f')
            force=1
            ;;
        'g')
            comp='gzip -c'
            ext='.gz'
            ;;
        'x')
            comp='xz -c'
            ext='.xz'
            ;;
        'h'|'?')
            show_help
            ;;
        :)
            echo "Option -${OPTARG} requires an argument."
            exit 1
            ;;
        *)
            echo "Invalid option: -${OPTARG}."
            exit 2
            ;;
    esac
done
shift $( expr $OPTIND - 1 )

[ -d $backuptarget ] || mkdir -p $backuptarget

for db in $databases
do
    printf -v filename "$backuptarget/$dbbbfname" $db
    if [ -f $filename ]
    then
        if [ $force -le 0 ]
        then
            echo "File $filename exists and -f isn't given - skipping"
            continue
        fi
    fi
    $mxdbdump --databases $db | $comp > $backuptarget/$db.sql$ext
done
