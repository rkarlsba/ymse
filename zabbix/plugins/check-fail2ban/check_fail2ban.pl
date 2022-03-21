#!/usr/bin/perl
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker

use strict;
use warnings;

my ($currently_failed, $total_failed, $currently_banned, $total_banned);

my $cmd = "fail2ban-client status sshd";
open my $fd,"$cmd|";
while (my $line = <$fd>) {
    if ($line =~ /Currently failed:\s+(\d+)/) {
        $currently_failed = $1;
    }
    elsif ($line =~ /Total failed:\s+(\d+)/) {
        $total_failed = $1;
    }
    elsif ($line =~ /Currently banned:\s+(\d+)/) {
        $currently_banned = $1;
    }
    elsif ($line =~ /Total banned:\s+(\d+)/) {
        $total_banned = $1;
    }
}


print "Currently failed: $currently_failed\n";
print "Total failed $total_failed\n";
print "Currently banned $currently_banned\n";
print "Total banned $total_banned\n";
