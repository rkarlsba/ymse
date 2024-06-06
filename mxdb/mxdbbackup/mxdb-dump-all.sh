#!/bin/bash
# vim:ts=4:sw=4:sts=4:et:ai

databases=$( echo show databases | mysql | egrep -v "^database|^lost+found|^information_schema|^performance_schema" )
backuptarget="/var/backups/mariadb"
mxdbdump="mariadb-dump" # or "mysqldump" or whatever comes next
dbbbfname="%s-$( date "+%Y-%m-%d" ).sql"

function show_help {
    [ $# -gt 0 ] && echo $@
    echo <<EOT
Syntax: $0 [-xz|-gz|-bz2|-h]
EOT
}

# echo /var/backups/$( date "+%Y/%m/%d/%H/%M/%S" )/synopsis.sql.xz

case $# in
    0)
        comp='cat'
        ext=''
        ;;
    1)
        case $1 in
            '-xz')
                comp='xz'
                ext='.xz'
                ;;
            '-gz')
                comp='gz'
                ext='.gz'
                ;;
            '-bz'|'-bz2')
                comp='bz2'
                ext='.bz2'
                ;;
            '-h'|'--help')
                show_help
                ;;
            *)
                echo "Unknown/invalid argument '$1' - exiting" >&2
                exit 1
                ;;
        esac
        ;;
    *)
        echo "Invalid number of arguments - exiting" >&2
        exit 2
        ;;
esac

[ -d $backuptarget ] || mkdir -p $backuptarget

for db in $databases
do
    printf -v filename "$backuptarget/$dbbbfname" $db
    if [ $xz -gt 0 ]
    then
        comp='xz -c'
    fi
    mysqldump $db | $comp > $backuptarget/$db.sql$ext
done
