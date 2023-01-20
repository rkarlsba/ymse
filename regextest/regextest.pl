#!/usr/bin/perl
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker

use strict;
use warnings;

my $infn = 'regextest.txt';

open($inf, $infn) || die "Can't open input file $inf: $!\n";

while (my $line = <$inf>) {
    print;
}

close($inf)
