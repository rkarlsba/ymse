#!/usr/bin/perl
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker

use strict;
use warnings;
use Data::Dumper;

=pod

Template App FTP Service	Hosts	Items 1	Triggers 1	Graphs	Dashboards	Discovery	Web					
Template App HTTP Service	Hosts	Items 1	Triggers 1	Graphs	Dashboards	Discovery	Web 1					
Template App HTTPS Service	Hosts	Items 1	Triggers 1	Graphs	Dashboards	Discovery	Web					
Template App IMAP Service	Hosts 1	Items 1	Triggers 1	Graphs	Dashboards	Discovery	Web					

* Lines should be kept as is, but the strings Hosts, Items 1, Triggers 1,
  Graphs, Dashboards, Discovery and Web should be removed.
* SOL, EOL and tabs to be replaced with | for this to be a markdown table.
* Two or more spaces should be merged into a single one.

=cut

my $infile = "oldshite.txt";
my $firstline = 1;
my $output;

open(my $fd, "<", $infile) || die "Can't open file '$infile': $!\n";

while (my $s = <$fd>) {
    chomp($s);
    my $line = "| ";
    my @splat = split(/\t/, $s);
    if ($firstline) {
        $output = "| Template name | Hosts | Items | Triggers | Graphs | Dashboards | Discovery | Web | Vendor | Version | Linked templates | Linked to templates | Tags\n";
        $firstline = 0;
        next;
    }
    my $servicename = $splat[0];
    $line = "| $servicename |";
    shift @splat;
    foreach my $field (@splat) {
        my $num = 0;
        if ($field =~ /^\w+\s+(\d+)$/) {
            $num = $1;
        }
        $line .= " $num |"
    }
    $line .= "\n";
    $output .= $line;
}
print $output;
