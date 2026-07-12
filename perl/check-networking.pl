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
use Getopt::Long;

# }}}
# report(str, frame); {{{

sub report {
    my $s = shift;
    my $f = shift;
    chomp($s);

    my $stjerner = length($s) + 4;

    if ($f > 0) {
        print "*" x $stjerner . "\n";
        print "* $s *\n";
        print "*" x $stjerner . "\n";
    } else {
        print "$s\n";
    }
}

# }}}
# Globals {{{

my $dev = "enp1s0";
my $frame = 0;
my $state = undef;
my $verbose = 0;

# }}}
# Main code {{{

Getopt::Long::Configure('bundling');

GetOptions(
    'dev|d=s'    => \$dev,
    'frame|f'    => \$frame,
    'verbose|v+' => \$verbose,
) or die "Invalid argument\n";

my $cmd = "ip addr list dev $dev";

if ($^O ne "linux") {
    print "We don't like $^O! Go away!\n";
    exit 1;
}

print "Probing device $dev\n" if ($verbose > 1);
print "With command \"$cmd\"\n" if ($verbose > 2);

open my $p,"$cmd|" || die "Can't run command \"$cmd\": $!";
while (<$p>) {
    # 1: lo: <LOOPBACK,UP,LOWER_UP> mtu 65536 qdisc noqueue state UNKNOWN group default qlen 1000
    if (/\d+:\s+$dev:.*?state\s+(\w+)/) {
        $state = $1;
        print "State for $dev is $state\n" if ($verbose > 2);
    }
}

if (!defined($state)) {
    report "Can't read state - wrong NIC name?\n", $frame;
    exit 1;
} elsif ($state eq "UP") {
    report "Networking is UP and everyone except Haaland et al. are happy\n", $frame if ($verbose > 0);
    exit 0;
} elsif ($state eq "DOWN") {
    report "Networking is DOWN, probably because the sysadmin disabled the NIC in virt-manager or somewhere\n", $frame;
} elsif ($state eq "UNKNOWN") {
    report "Networking is UNKNOWN, possibly because you're trying to probe lo or something is on bad mushrooms\n", $frame;
} else {
    exit 1;
}

# }}}
