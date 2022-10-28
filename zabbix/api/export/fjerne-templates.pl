#!/usr/bin/perl
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker

use strict;
use warnings;

my $infn="zbx_export_hosts.xml";
my $outfn="zbx_export_hosts-vaska.xml";

open(my $inf,$infn) || die "$!";
open(my $outf,">$outfn") || die "$!";

=head {{{
        <host>
            <host>adconnect-01.ada.hioa.no</host>
            <name>adconnect-01.ada.hioa.no</name>
            <description>HighAvailability for AzureAD Sync</description>
            <templates>
                <template>
                    <name>Template OS Windows</name>
                </template>
            </templates>
            <groups>
                <group>
=cut }}}

my $template = 0;
while (my $line = <$inf>) {
    if ($line =~ /^\s+<template>/) {
        $template = 1;
    } elsif ($line =~ /^\s+<\/template>/) {
        $template = 0;
        next;
    }
    print $outf $line unless ($template);
}

close($inf);
close($outf);
