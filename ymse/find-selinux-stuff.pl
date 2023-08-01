#!/usr/bin/perl
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker

use strict;
use warnings;

my $infile = '/var/log/messages';
my %results;
my $f;

die("Something went south: $!\n") unless open ($f,'<', $infile);

while (<$f>) {
    next unless (/SELinux is preventing ([\/\w\.]+) from (\w+) access on the file ([\/\w\.]+)/);
    my $entry = "SELinux is preventing $1 from $2 access on the file $3";
    $results{$entry}++;
}

foreach my $s (sort keys %results) {
    print "$s\t$results{$s}\n";
}
