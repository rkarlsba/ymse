#!/usr/bin/perl
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker
#
# Find SAS disk issues - by Roy Sigurd Karlsbakk <roy@karlsbakk.net>
# Copyleft 2022-05-04. Licensed under AGPL.

use strict;
use warnings;

# Example {{{
#
# ==================== /dev/sda ====================
# smartctl 7.2 2020-12-30 r5155 [x86_64-linux-5.10.0-12-amd64] (local build)
# Copyright (C) 2002-20, Bruce Allen, Christian Franke, www.smartmontools.org
#
# === START OF READ SMART DATA SECTION ===
# Error counter log:
#           Errors Corrected by           Total   Correction     Gigabytes    Total
#               ECC          rereads/    errors   algorithm      processed    uncorrected
#           fast | delayed   rewrites  corrected  invocations   [10^9 bytes]  errors
# read:   3716474487        0         0  3716474487          0     170154.272           0
# write:         0        0         0         0          0      35743.185           0
#
# Non-medium error count:        3
#
# [GLTSD (Global Logging Target Save Disable) set. Enable Save with '-S on']
#
# }}}

my $filename = "4T-disks-in-mikro.txt"
my $found = 0;
my $read = 0;
my $write = 0;
my $err = 0;
my $rline = undef;
my $wline = undef;
my $dev = undef;
my $headerprinted = 0;

sub printheader {
    print <<EOT
=== START OF READ SMART DATA SECTION ===
Error counter log:
          Errors Corrected by           Total   Correction     Gigabytes    Total
              ECC          rereads/    errors   algorithm      processed    uncorrected
          fast | delayed   rewrites  corrected  invocations   [10^9 bytes]  errors
EOT
}

open my $f,"<", $filename ||
    die "Can\t open file '$filename': $!\n";

while (my $line = <$f>) {
    chomp($line);
    unless ($found) { # Device name not found
        if ($line =~ /(\/dev.*?)\s/) {
            $found = 1;
            $read = 0;
            $write = 0;
            $err = 0;
            $rline = undef;
            $wline = undef;
            $dev = $1;
            next;
        }
    } else { # Device name found
        #read:   1111510796        0         0  1111510796          0       4289.460           0
        #write:         0        0         0         0          0       6368.751           0
        if ($line =~ /^read:\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+\.\d+)\s+(\d+)/) {
            $rline = "[R] $line\n";
            $read = 1;
            $err++ if ($5 > 0);
        } elsif ($line =~ /^write:\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+\.\d+)\s+(\d+)/) {
            $wline = "[W] $line\n";
            $write = 1;
            $err++ if ($5 > 0);
        }
        if ($read and $write) {
            if ($err) {
                unless ($headerprinted) {
                    &printheader;
                    $headerprinted++;
                }
                print "============ $dev ============\n";
                print $rline;
                print $wline;
            }
            $found = 0;
        }
    }
}

