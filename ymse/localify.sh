#!/bin/bash
# vim:ts=4:sw=4:sts=4:et:ai

# X11@  X11R6@  bin/  lib/  libexec/  local/  sbin/  share/  standalone/

die() {
    case $# in
        1)
            errmsg=$1
            exitcode=254
            ;;
        2)
            errmsg=$1
            exitcode=$2
            if $( echo "$exitcode" | grep -P '^[0-9]+$' > /dev/null )
            then
                echo 'number'
            else
                echo 'not number'
                exitcode=253
            fi
            ;;
        *)
            errmsg="Unknown error"
            exitcode=255
            ;;
    esac

    echo "$errmsg" 1>&2
    exit $exitcode
}

targetpath=$1

[ -d $targetpath ] || die "Target path '$targetpath' not found" 1
[ -w $targetpath ] || die "Target path '$targetpath' not writable" 2

find $targetpath -type f -exec grep -wHP '/usr/(bin|lib|libexec|sbin|share)' {} \;

