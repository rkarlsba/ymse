#!/usr/bin/perl
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker

use strict;
use warnings;

my $infn = 'regextest.txt';
my $inf;
my $line;
my $searchstring = 'Roy';

open($inf, $infn) || die "Can't open input file $inf: $!\n";

while ($line = <$inf>) {
    chomp($line);
    if ($line =~ /\b($searchstring)\b/) {
        print "String \"$searchstring\" found in line \"$line\"\n";
    }
    #print("$line\n");
}

close($inf)
