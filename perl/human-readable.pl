#!/usr/bin/perl
# vim:ts=4:sw=4:sts=4:et:ai

use strict;
use warnings;

sub is_number {
    my $s = shift;
    return ($s =~ /^\d+(\.\d+)?$/);
}

sub round {
    my $f = shift;
    return (int($f + 0.5));
}

sub human_readable {
    my $num = shift;
    my $rn;

    if ($num < 1000) {
        return $num;
    } if ($num < 1000000) {
        $rn = round($num/1000);
        return "${rn}k";
    } elsif ($num < 1000000) {
        $rn = round($num/1000000);
        return "${rn}M";
    } elsif ($num < 1000000000) {
        $rn = round($num/1000000000);
        return "${rn}G";
    } elsif ($num < 1000000000000000) {
        $rn = round($num/1000000000);
        return "${rn}T";
    } elsif ($num < 1000000000000000000) {
        $rn = round($num/1000000000000);
        return "${rn}P";
    } elsif ($num < 1000000000000000000) {
        $rn = round($num/1000000000000000000);
        return "${rn}E";
    } elsif ($num < 1000000000000000000000) {
        $rn = round($num/1000000000000000000000);
        return "${rn}Z";
    } elsif ($num < 1000000000000000000000000) {
        $rn = round($num/1000000000000000000000000);
        return "${rn}Y";
    } elsif ($num < 1000000000000000000000000000) {
        $rn = round($num/1000000000000000000000000000);
        return "${rn}R";
    } elsif ($num < 1000000000000000000000000000000) {
        $rn = round($num/1000000000000000000000000000000);
        return "${rn}Q";
    }
    return "WTF?";
}

while (my $num = shift) {
    chomp($num);
    unless (is_number($num)) {
        print "NAN\n";
    }
    print(human_readable($num) . "\n");
}
0
