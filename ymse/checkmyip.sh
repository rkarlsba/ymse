#!/bin/bash
# vim:ts=4:sw=4:sts=4:et:ai

exitcode=1
ip4='Unknown error occured'
ip6='Unknown error occured'

case "$1" in
    "-4")
        ip4=$( curl -s https://ip4.karlsbakk.net/ip.php ) && exitcode=0 || ip4='Curl returned $? on IPv4 lookup, so something is barfy'
        echo $ip4
        ;;
    "-6")
        ip6=$( curl -s https://ip6.karlsbakk.net/ip.php ) && exitcode=0 || ip6='Curl returned $? on IPv6 lookup, so something is barfy'
        echo $ip6
        ;;
    *)
        echo "Syntax: $0 < -4 | -6 >" >&2
        echo >&2
        echo "Where -4 looks up the iPv4 address and -6 the IPv6 address." >&2
        echo "If the given address cannot be found or other known errors occur, this script" >&2
        echo "returns a non-zero exitcode" >&2
        ;;
esac

exit $exitcode
