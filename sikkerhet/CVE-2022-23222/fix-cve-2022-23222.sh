#!/bin/bash
# vim:ts=4:sw=4:sts=4:et:ai

procfilename='/proc/sys/kernel/unprivileged_bpf_disabled'

function softclose {
    if $( echo 2 > $procfilename 2>/dev/null )
    then
        echo Closed
    else
        echo Closing failed: $!
        return 1
    fi
    return 0
}

function hardclose {
    if $( echo 1 > $procfilename 2>/dev/null )
    then
        echo Closed
    else
        echo Closing failed: $!
        return 1
    fi
    return 0
}

status=$( cat $procfilename )

case "$status" in
    0)
        echo "It's all open - closing down to 2 (soft close)"
        if softclose
        then
            echo "Closed soft - this can be opened again"
        else
            echo "try hardclose"
            if hardclose
            then
                echo "Closed hard - this can only be opened again with a reboot"
            fi
        fi
        ;;
    1)
        echo "It's all closed - hard\n"
        ;;
    2)
        echo "It's all closed - soft\n"
        ;;
    *)
        echo "This [$status] is all greek to me\n" >&2
        exit 1
        ;;
esac

