#!/bin/bash
# vim:ts=4:sw=4:sts=4:et:ai:si:fdm=marker:tw=80

if [ $# -ne 1 ]
then
    echo "Syntax: $0 hostname" >&2
    exit 1
fi

HOSTNAME=$1

curl -s https://$HOSTNAME/asdf/2022/02/03/14071https:/uni.os > /dev/null 2>&1
retcode=$?

# Below, 0 is normally ok, but in our case, we don't want curl to reurn ok,
# since that means the system is vulnerable to the attack. In our 'solution' to
# the attack, was to use nginx' HTTP 444, which isn't an HTTP status code at
# all, only a way to terminate the HTTP request abrutly. When this happens to
# curl, it exits with exitcode 52: "The server did not reply anything, which
# here is considered an error." To do this with Apache, you'll need to do
# something like this
#
#        SecRule RESPONSE_STATUS 444 "id:'444444',phase:3,log,drop"
#
case $retcode in
    0)
        echo "ERROR: Server $HOSTNAME is vurlerable to this attack"
        ;;
    6)
        echo "WARNING: Could not resolve host "$HOSTNAME""
        ;;
    52)
        echo "OK: Server $HOSTNAME is not vurlerable to this attack"
        ;;
    *)
        echo "WARNING: Server $HOSTNAME might be vulnerable or on the piss, curl gave me [$retcode]"
        ;;
esac

exit $retcode

