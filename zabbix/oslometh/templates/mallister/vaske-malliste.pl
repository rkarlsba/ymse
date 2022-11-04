#!/usr/bin/perl
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker

use strict;
use warnings;

=begin {{{
Name	Hosts	Applications	Items	Triggers	Graphs	Screens	Discovery	Web	Linked templates	Linked to templates	Tags
Infopi	Hosts 24	Applications 11	Items 55	Triggers 34	Graphs 7	Screens	Discovery 2	Web	Template OS Linux		
JKS certificate expiry check	Hosts	Applications 1	Items 1	Triggers 2	Graphs	Screens	Discovery	Web			
LLD SNMP - DELL CMC	Hosts 2	Applications 6	Items 27	Triggers 16	Graphs	Screens	Discovery 3	Web			
OS Linux - OsloMet custom	Hosts	Applications 2	Items 8	Triggers 8	Graphs	Screens 1	Discovery 2	Web			
PostgreSQL	Hosts 9	Applications 2	Items 40	Triggers 11	Graphs 8	Screens 2	Discovery 1	Web			
Prosesstellekilling	Hosts	Applications	Items 5	Triggers	Graphs	Screens	Discovery	Web			
Raritan EMX SNMP environmental probe	Hosts 3	Applications 3	Items 13	Triggers 5	Graphs	Screens	Discovery	Web	Template Module Generic SNMPv2		
Template Adobe sync log monitorering	Hosts 1	Applications 1	Items 5	Triggers 1	Graphs	Screens	Discovery	Web			
Template App Apache by HTTP	Hosts 6	Applications 2	Items 22	Triggers 5	Graphs 3	Screens 1	Discovery 1	Web			
=cut }}}

open(my $inf,"malliste.csv") || die "$!";

while (<$inf>) {
    chomp();
    s/\s*Applications.*//;
    #print "$_\n";
    my ($name,$hosts) = split(/\t/);
    next if ($name eq 'Name' and $hosts eq 'Hosts');
    $hosts = 0 if ($hosts eq 'Hosts');
    $hosts =~ s/Hosts //;
    print("$name\t$hosts\n");
}
