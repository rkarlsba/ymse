#!/usr/bin/perl
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker

use strict;
use warnings;

my $maskinliste = "roysk.txt";
my $ouiliste = "oui.txt";
my $args = @ARGV;
my $liste = undef;

if ($args > 0 and $ARGV[0] eq '-') {
    print("Read from stdin...\n");
    $liste = *STDIN;
    # || die ("wtf? $!\n");
} else {
    print("open $maskinliste...\n");
    open($liste, $maskinliste) || die ("wtf? $!\n");
}

while (my $line = <$liste>) {
    chomp($line);
    $line = lc(<$line>);
}
