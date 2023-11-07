#!/bin/bash
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker

# Variables
snapname="$( date +%F_%T.snap | sed s/:/-/g)"
debug=0

# Functions
vm_exists() { # {{{
    if [ $# -ne 1 ]
    then
        printf "RTFS\n" >&2
        exit 10
    fi
    _vm=$1
    if $( virsh list --all | awk '/running|idle|paused|in shutdown|shut off|crashed|pmsuspended/ { print $2 }' | grep "$vm" > /dev/null 2>&1 )
    then
        return 0
    fi
    return 1
}
# }}}
vm_has_state() { # {{{
    if [ $# -ne 2 ]
    then
        printf "RTFS\n" >&2
        exit 11
    fi
    _vm=$1
    _state=$2
    if $( virsh list --all | awk '/' . $2 . '/ { print $2 }' | grep "$vm" > /dev/null 2>&1 )
    then
        return 0
    fi
    return 1
}
# }}}
vm_running() { # {{{
    return vm_has_state running
}
# }}}
vm_shut_off() { # {{{
    has_state 'shut off'
}
# }}}
debprint() { # {{{
    if [ $debug -eq 0 ]
    then
        return
    fi

    printf "$@"
}
# }}}

# Initialisation
if [ "$UID" -ne 0 ]
then
    printf "This must be run as root\n" >&2
    exit 1
fi

if [ $# -le 0 ]
then
    printf "Syntax: $0 <vm> [vm [vm [...]]]\n" >&2
    exit 2
fi

# Are we debugging?
while [ "$1" = "-d" ]
do
    debug=$(( $debug + 1 ))
    shift
done

# Main loop
for vm in "$@"
do
    if vm_exists $1
    then
        debprint "Snapshot $vm\n"
        if [ $debug -gt 0 ]
        then
            echo virsh snapshot-create-as \
                --name \"$snapname\" \
                --disk-only \
                --domain \"$vm\"
        else
            virsh snapshot-create-as \
                --name "$snapname" \
                --disk-only \
                --domain "$vm"
        fi
    else
        printf "Named VM "$vm" does not exist\n" >&2
    fi
done

exit 99

