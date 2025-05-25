#!/usr/bin/perl
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker

use strict;
use warnings;

my %roles;
my $debug = 1;
my ($fh,$fn);

sub debprint {
    return unless ($debug);
    print("[DEBUG] @_");
}

while ($fn = shift) {
    unless (open($fh, "<", $fn)) {
        print STDERR "Can't open file '$fn': $!\n";
        next;
    }

    while (my $s = <$fh>) {
        if ($s =~ /ALTER TABLE .*? OWNER TO ([a-zA-Z0-9_\-]+);/) {
            $roles{$1}++;
        }
    }
    close ($fh);
}

foreach my $role (keys %roles) {
    print "$role\n";
}
