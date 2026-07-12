#!/usr/bin/perl
# Vim modeline {{{

# vim:ts=4:sw=4:sts=4:et:ai:si:fdm=marker

# }}}
# Debug output {{{
#
# Når nettet er oppe:
#
# 2: enp1s0: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc fq_codel state UP group default qlen 1000
#     link/ether 52:54:00:15:6c:36 brd ff:ff:ff:ff:ff:ff
#     altname enx525400156c36
#     inet 192.168.10.78/24 brd 192.168.10.255 scope global noprefixroute enp1s0
#        valid_lft forever preferred_lft forever
#     inet6 2a01:799:d66:4000::78/64 scope global noprefixroute
#        valid_lft forever preferred_lft forever
#     inet6 fe80::89c6:b616:4554:37cb/64 scope link noprefixroute
#        valid_lft forever preferred_lft forever
#
# Når det er nede:
#
# 2: enp1s0: <NO-CARRIER,BROADCAST,MULTICAST,UP> mtu 1500 qdisc fq_codel state DOWN group default qlen 1000
#     link/ether 52:54:00:15:6c:36 brd ff:ff:ff:ff:ff:ff
#     altname enx525400156c36
# }}}
# Libs and such {{{

use strict;
use warnings;

# }}}
# report(str); {{{

sub report {
    my $s = shift;
    chomp($s);

    my $stjerner = length($s) + 4;

    print "*" x $stjerner . "\n";
    print "* $s *\n";
    print "*" x $stjerner . "\n";
}

# }}}
# Globals {{{

my $dev = "enp1s0";
my $cmd = "ip addr list dev $dev";
my $state = undef;

# }}}
# Main code {{{

if ($^O ne "linux") {
    print "We don't like $^O! Go away!\n";
    exit 1;
}

open my $p,"$cmd|" || die "Can't run command \"$cmd\": $!";
while (<$p>) {
    if (/\d+:\s+$dev:.*?state\s+(\w+)/) {
        $state = $1;
    }
}

if (!defined($state)) {
    report "Can't read state - wrong NIC name?\n";
    exit 1;
} elsif ($state eq "UP") {
    report "Networking is UP and everyone except Haaland are happy\n";
    exit 0;
} elsif ($state eq "DOWN") {
    report "Networking is DOWN, probably because the sysadmin disabled the NIC in virt-manager or somewhere\n";
} else {
    exit 1;
}

# }}}
