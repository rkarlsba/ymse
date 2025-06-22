#!/bin/bash
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker

case $1 in
    stop)
        # Stop services and sockets, starting with the latter
        for svc in $( systemctl -a | awk '/sympa/ { print $1 }' | sort -k 2 -t . -r )
        do
            systemctl stop $svc
        done
        ;;
    start)
        # Start services and sockets, starting with the former
        for svc in $( systemctl -a | awk '/sympa/ { print $1 }' | sort -k 2 -t . )
        do
            systemctl start $svc
        done
        ;;
    restart)
        # Stop, and if then start
        $0 stop
        $0 start
        ;;
    "nothing else")
        echo "You're sick!"
        ;;
    *)
        echo "This script is for hanling the Sympa processes. Please use $0 [stop|start|restart] and nothing else"
        ;;
esac
