#!/usr/bin/perl
# vim:ts=4:sw=4:sts=4:et:ai
#
# solr02-stage.oslomet.no has OS Linux solr02-stage.oslomet.no 4.18.0-305.25.1.el8_4.x86_64 #1 SMP Mon Oct 18 14:34:11 EDT 2021 x86_64
# fildeling.oslomet.no has OS Linux fildeling.oslomet.no 4.18.0-348.12.2.el8_5.x86_64 #1 SMP Mon Jan 17 07:06:06 EST 2022 x86_64
# mdb01-stage.oslomet.no has OS Linux mdb01-stage.oslomet.no 4.18.0-305.25.1.el8_4.x86_64 #1 SMP Mon Oct 18 14:34:11 EDT 2021 x86_64
#

use strict;
use warnings;

my $min_major = 5;
my $min_minor = 8;
my ($major, $minor, $patch);

while (<STDIN>) {
    if (/^([0-9a-z\-\.]+) has OS Linux ([0-9a-z\-\.]+) (\d+)\.(\d+)\.(\d+)/) {
        ($major, $minor, $patch) = ($3,$4,$5);
        if ($major >= $min_major and $minor >= $min_minor) {
            print "$1\t$major.$minor.$patch\n";
        }
    }
}
