#!/bin/bash
# vim:ts=4:sw=4:sts=4:et:ai:si:fdm=marker

IMAPv4="158.36.0.140"
IMAPv6="2001:700:700:2000::140"

# Either REJECT or DROP the outgoing packages. REJECT replies with a TCP RST
# package, while DROP just drops the package. Obviously, REJECT will trigger the
# error far earlier, but DROP may better to simulate an error.
TARGET="REJECT" # Either REJECT or DROP. The former makes

feilfeilfeil() {
    echo "$@" >&2
    exit 1
}

case "$1" in
    block)
        iptables -I OUTPUT -d $IMAPv4 -j $TARGET || feilfeilfeil "Something weird went wrong (1)"
        ip6tables -I OUTPUT -d $IMAPv6 -j $TARGET || feilfeilfeil "Something weird went wrong (2)"
        ;;
    unblock)
        iptables -D OUTPUT -d $IMAPv4 -j $TARGET || feilfeilfeil "Something weird went wrong (3)"
        ip6tables -D OUTPUT -d $IMAPv6 -j $TARGET || feilfeilfeil "Something weird went wrong (4)"
        ;;
    *)
        feilfeilfeil "Syntax $0 <block | unblock>" >&2
        ;;
esac

exit 0
