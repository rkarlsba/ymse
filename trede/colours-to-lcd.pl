#!/usr/bin/perl
# Docs and license info - if in vim, press <space> to open {{{
#
# vim:ts=4:sw=4:sts=4:et:ai:tw=80:fdm=marker
# 
# This scripts first removes initial M600, since it considers the printer to
# have the relevant colour loaded from the start. Then it chews through the rest
# of the file, looking for lines like
#
# M600
# T0
#
# Then it adds a M117 before the M600, telling the user which colour is needed,
# according to the ones set in the hash below.
#
# Written by Roy Sigurd Karlsbakk <roy@karlsbakk.net> and licensed under AGPL
# v3. See https://www.gnu.org/licenses/agpl-3.0.html for details.
#
# }}}

use strict;
use warnings;

my %colour = (
    'T0' => 'White',
    'T1' => 'Black',
    'T2' => 'Red',
    'T3' => 'Green',
    'T4' => 'Blue',
);

my $initial_M600 = 0;
my $tee;

while (my $line = <STDIN>) {
    if ($line =~ /^M600/) {
        # Discard the initial M600
        unless ($initial_M600) {
            $initial_M600++;
            next;
        }
        # This is M600 #2 or later, so read the next line with the Tsomething
        $line = <STDIN>;
        # If the line doesn't contain Tsomething, ignore it and go on
        if ($line =~ /^(T\d+)/) {
            $tee = $1;
            print "M117 [$tee] Insert $colour{$tee}, please\n";
        }
        print "M600\n";
        print "$tee\n";
    } else {
        print "$line";
    }
}

