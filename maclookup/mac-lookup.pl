#!/usr/bin/perl
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker

use strict;
use warnings;

my $maskinliste = "roysk.txt";
my $ouiliste = "oui.txt";
my ($ip,$mac,$macprefix,$count,$len,$vendor,$pvendor) = undef;
my @mac = undef;
my $liste = undef;

# {{{
# 158.36.144.254  e4:fc:82:e6:38:08   4283  256980  Juniper Networks
# 158.36.144.253  94:bf:94:3d:60:1c   1602   96120  Juniper Networks
# 192.168.10.1    50:eb:f6:5c:15:55      1      60  Unknown vendor
# 192.168.49.1    ac:9b:0a:e7:1a:99      1      60  Sony Corporation
# 158.36.144.195  bc:e9:2f:a5:08:50      5     300  HP Inc.
# 158.36.144.162  72:5c:fc:7b:3f:0f     27    1134  Unknown vendor
# 192.168.80.1    80:e8:2c:ed:55:28      1      60  Hewlett Packard
# 192.168.97.1    50:eb:f6:5c:15:55      1      60  Unknown vendor
# 192.168.122.1   b0:5c:da:38:9c:cf      1      60  HP Inc.
# }}}

my $args = @ARGV;
if ($args > 0 and $ARGV[0] eq '-') {
    $liste = *STDIN;
    # || die ("wtf? $!\n");
    print("Read from stdin...\n");
} else {
    open($liste, $maskinliste) || die ("wtf? $!\n");
    print("open $maskinliste...\n");
}

while (my $line = <$liste>) {
    chomp($line);
    $line = lc(<$line>);
    if ($line =~ /^\s*(\d+\.\d+\.\d+\.\d+)\s+([0-9a-f]{2})[\:\-]?([0-9a-f]{2})[\:\-]?([0-9a-f]{2})[\:\-]?([0-9a-f]{2})[\:\-]?([0-9a-f]{2})[\:\-]?([0-9a-f]{2})\s+(\d+)\s+(\d+)\s+(.*)/) {
        ($ip,$count,$len,$vendor) = ($1,$8,$9,$10,$11);
        @mac = ($2,$3,$4,$5,$6,$7);
    } elsif ($line =~ (/([0-9a-f]{2})[\:\-]([0-9a-f]{2})[\:\-]([0-9a-f]{2})[\:\-]([0-9a-f]{2})[\:\-]([0-9a-f]{2})[\:\-]([0-9a-f]{2})/)) {
        @mac = ($1,$2,$3,$4,$5,$6);
    } else {
        print("Doesn't match anything: '$line'\n");
        next;
    }
    # MAC address is defined at this point
    $mac = join(':', @mac);
    $macprefix = join(':', @mac[0..2]);
    if (defined($ip)) {
        print "$ip\t$mac\t$count\t$len\t$vendor\t($macprefix)\n";
    } else {
        print "$mac\t$vendor\n";
    }
}
