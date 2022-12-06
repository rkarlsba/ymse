#!/usr/bin/perl
# vim:ts=4:sw=4:sts=4:et:ai

use strict;
use warnings;
use Data::Dumper;

=pod
Personalities : [raid1] [linear] [multipath] [raid0] [raid6] [raid5] [raid4] [raid10]
md1 : active raid6 sdc[9] sdn[10] sdi[13] sdj[14] sda[7] sdf[11] sde[15] sdd[8] sdb[16]
      15627063296 blocks super 1.2 level 6, 512k chunk, algorithm 2 [10/8] [_UUUUUUU_U]
      [=====>...............]  recovery = 25.2% (493554884/1953382912) finish=384.4min speed=63284K/sec
      bitmap: 15/15 pages [60KB], 65536KB chunk

md3 : active raid1 sdg1[3] sdm1[2]
      116176768 blocks super 1.2 [2/2] [UU]
=cut

my $debug = 1;
my $mdstat_fn = '/proc/mdstat';

if ($debug gt 0) {
    $mdstat_fn = '../mdstat-test.txt';
}

open my $mdstat,"cat $mdstat_fn|" || die "kan ikke lese $mdstat_fn";
while (my $line = <$mdstat>) {
    chomp($line);
    next unless ($line =~ /^md[0-9]/);
    my @elements = split(/ /, $line);
    my $md = $elements[0];
    my @disks;
    my @spares;
    for (my $i = 4 ; ; $i++) {
        if ($elements[$i]) {
            my $d = $elements[$i];
            $d =~ s/\[\d+\]//;
            if ($d =~ /\(S\)/) {
                $d =~ s/\(S\)//;
                push @spares,$d;
            } else {
                push @disks,$d;
            }
        } else {
            last;
        }
    }
    print "$md:\n";
    print "Disks:\t" . join(' ', @disks) . "\n";
    print "Spares:\t" . join(' ', @spares) . "\n";
    # print Dumper(@disks);
    # print Dumper(@spares);
}
