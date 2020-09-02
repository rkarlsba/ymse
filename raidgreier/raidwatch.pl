#!/usr/bin/perl
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker

# raidwatch.pl
#
# Read and slightly parse /dev/md0 to output realtime data from /dev/mdstat for a particular md device
#
# Written by Roy Sigurd Karlsbakk <roy@karlsbakk.net>
# Licensed under AGPL v3.
#
# Example output {{{
=comment
Personalities : [raid1] [raid6] [raid5] [raid4] [linear] [multipath] [raid0] [raid10]                                                                                                                                                        │·············
md0 : active raid6 sdb[6] sdl[13] sdi[8] sda[2] sdn[9] sdp[14] sdd[5] sdk[10] sdf[7] sdo[16] sdg[0] sde[15] sdc[3] sdm[19] sdj[18](S) sdh[17] sdq[1] sds[4]
      109394518016 blocks super 1.2 level 6, 512k chunk, algorithm 2 [17/17] [UUUUUUUUUUUUUUUUU]
      [>....................]  reshape =  0.1% (10313788/7813894144) finish=2571.3min speed=50579K/sec
      bitmap: 1/59 pages [4KB], 65536KB chunk

md2 : active raid1 sdr5[0] sdt5[1]
      195379200 blocks super 1.2 [2/2] [UU]
      bitmap: 0/2 pages [0KB], 65536KB chunk

md1 : active raid1 sdr1[0] sdt1[1]
      781116416 blocks super 1.2 [2/2] [UU]
      bitmap: 3/6 pages [12KB], 65536KB chunk

unused devices: <none>
=cut
# }}}

use strict;
use warnings;

my $debug=1;
my $mdstat_fn = "/proc/mdstat";
my $raiddev;
my $raiddevs=0;

$mdstat_fn = "test-input.txt" if ($debug);

while ($raiddev = shift) {
    print("Got raid device '$raiddev'\n");
    $raiddevs++;
}
die ("You need to specify a raid device\n") unless ($raiddevs > 0);
die ("Useless device '$raiddev' given\n") unless ($raiddev =~ m/^\/dev\/md/);
die ("Device '$raiddev' is not a block device\n") unless (-b $raiddev);
warn ("Only one raid device supported so far. Ignoring all but the last.\n") if ($raiddevs > 1);

open(my $mdstat, "<", $mdstat_fn) || die "Can't open file '$mdstat_fn' for reading: $!\n";

while (my $line = <$mdstat>) {
    print "$line";
}
