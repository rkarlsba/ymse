#!/usr/bin/perl
# vim:ts=4:sw=4:sts=4:et:ai

use strict;
use warnings;

my $debug = 0; # set to 0 for a normal run
my $fn = '/proc/mdstat';
my $dev = shift;
my $found=0;

sub debprint {
    my $s = shift;
    $s = "¿qué?" unless (defined($s));
    print("[DEBUG] $s") if ($debug);
}

die("Syntax: $0 mdX\n") unless (defined($dev));
warn("Is '$dev' really an md device?\n") unless ($dev =~ /^md\d+$/);

$fn = 'mdstat.txt' if ($debug);

die("This must be run on Linux\n") if ($^O ne "Linux" and not $debug);

open(my $mdstat, "<", $fn) ||
    die("Failed to open file '$fn': $!\n");

debprint("Looking for '$dev' in the haystack…\n");

while (my $line = <$mdstat>) {
    chomp($line);
    debprint("Looking for line '$line'\n");
    if ($line =~ /^${dev}\s/) {
        $found=1;
    }
    if ($found) {
        print "$line\n";
        if ($line =~ /^\s*$/) {
            $found=0;
        }
    }
}

