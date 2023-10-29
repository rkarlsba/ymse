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

    if ($num < 10**3) {
        return $num;
    } if ($num < 10**6) {
        $rn = round($num/10**3);
        return "${rn}k";
    } elsif ($num < 10**9) {
        $rn = round($num/10**6);
        return "${rn}M";
    } elsif ($num < 10**12) {
        $rn = round($num/10**9);
        return "${rn}G";
    } elsif ($num < 10**15) {
        $rn = round($num/10**12);
        return "${rn}T";
    } elsif ($num < 10**18) {
        $rn = round($num/10**15);
        return "${rn}P";
    } elsif ($num < 10**21) {
        $rn = round($num/10**18);
        return "${rn}E";
    } elsif ($num < 10**24) {
        $rn = round($num/10**21);
        return "${rn}Z";
    } elsif ($num < 10**27) {
        $rn = round($num/10**24);
        return "${rn}Y";
    } elsif ($num < 10**30) {
        $rn = round($num/10**27);
        return "${rn}R";
    } elsif ($num < 10**33) {
        $rn = round($num/10**30);
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
