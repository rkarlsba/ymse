#!/bin/bash
# vim:ts=4:sw=4:sts=4:et:ai

databases=$( echo show databases | mysql | egrep -v "^database|^lost+found|^information_schema|^performance_schema" )
backuptarget="/var/backups/mariadb"
mxdbdump="mariadb-dump" # or "mysqldump" or whatever comes next
xz=0
dbbbfname="%s-$( date "+%Y-%m-%d" ).sql"
comp='cat'
ext=''

# echo /var/backups/$( date "+%Y/%m/%d/%H/%M/%S" )/synopsis.sql.xz

if [ "$1"="-xz" ]
then
    xz=1
fi

[ -d $backuptarget ] || mkdir -p $backuptarget

for db in $databases
do
    printf -v filename "$backuptarget/$dbbbfname" $db
    if [ $xz -gt 0 ]
    then
        comp='xz -c'
        ext='.xz'
    fi
    mysqldump $db | $comp > $backuptarget/$db.sql$ext
done
