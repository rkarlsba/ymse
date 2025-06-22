#!/usr/bin/perl
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker

use strict;
use warnings;

# ./singledevstatus.pl /dev/sd{a..e}

my $printline = 0;
my $header = 1;
my $smartctl = "/usr/sbin/smartctl";
my $proc_fd;

# Model Family:     Hitachi Deskstar 7K3000
# Device Model:     Hitachi HDS723020BLA642
# User Capacity:    2,000,398,934,016 bytes [2.00 TB]

foreach my $dev (@ARGV) {
    if ($dev =~ m/^\/dev\/[sh]d\w\w?/) {
        if ( ! -b $dev ) {
            print STDERR "Ignoring '$dev': Not a block device!\n";
            next;
        }
        my $cmd = "$smartctl -a $dev";
        if (!open $proc_fd, "$cmd|") {
            warn "Can't run command '$cmd': $!\n";
            next;
        }
        print "============================== $dev ==============================\n";
        while (my $line = <$proc_fd>) {
            chomp($line);
            if ($line =~ /^Model Family|Device Model|User Capacity/) {
                print("$line\n");
            } elsif ($printline) {
                if ($line =~ /^\s*$/) {
                    $printline = 0;
                } else {
                    print("$line\n");
                }
            } else {
                if ($line =~ /ID\#\s/) {
                    $printline =1;
                }
            }
        }
        close $proc_fd;
    }
}

