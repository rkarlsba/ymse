#!/bin/bash
# Docs {{{
# 
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker:tw=80
#
# Sandkasse
#
# Written by Roy Sigurd Karlsbakk <roy@karlsbakk.net> CopyLeft 2024.
# Licensed under som obscure BSD license of choice
#
# }}}
# Global Variables {{{

verbose=0
TRUE=0
FALSE=1

# }}}
# function file_is_empty {{{

function file_is_empty {
    filename=$1

    if [ -f $filename -a ! -s $filename ]
    then
        [ $verbose -gt 0 ] && echo "File $filename exists, seems to be an ordinary file and is zero bytes long"
        return $TRUE
    fi
    return $FALSE
}

# }}}
# Main program {{{

for f in "$@"
do
    if file_is_empty $f
    then
        echo "file '$f' is indeed empty"
    else
        echo "file '$f' is not empty"
    fi
done

# }}}
