#!/usr/bin/perl
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker

use strict;
use warnings;

my $maskinliste = "roysk.txt";
my $ouiliste = "oui.txt";
my ($ip,$mac,$count,$len,$vendor,$pvendor);
my $liste;

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

open($liste, $maskinliste) || die ("wtf? $!\n");
while (<$liste>) {
    chomp();
    if (/^\s+(\d+\.\d+\.\d+\.\d+)\s+([0-9a-z]{2}[\:\-][0-9a-z]{2}[\:\-][0-9a-z]{2}[\:\-][0-9a-z]{2}[\:\-][0-9a-z]{2}[\:\-][0-9a-z]{2})\s+(\d+)\s+(\d+)\s+(.*)/) {
        ($ip,$mac,$count,$len,$vendor) = ($1,$2,$3,$4,$5);
    }
    my $macprefix = $mac;
    #$macprefix =~ s/([0-9a-z]{2})\:([0-9a-z]{2})\:([0-9a-z]{2}.*/)/$1$2$3/);
    $macprefix =~ s/://g;
    my $macpre = uc(substr($macprefix, 0, 6));
    print "$ip\t$mac\t$count\t$len\t$vendor\t($macprefix - $macpre)\n";
}
