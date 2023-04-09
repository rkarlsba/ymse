#!/usr/bin/perl
# vim:ts=4:sw=4:sts=4:et:ai

use strict;
use warnings;

my $debug = 1; # set to 0 for a normal run
my $fn = '/proc/mdstat';

$fn = 'mdstat.txt' if ($debug);

die("This must be run on Linux\n") if ($^O ne "Linux");

open(my $mdstat, "<", $fn) ||
    die("Failed to open file '$fn': $!\n");


