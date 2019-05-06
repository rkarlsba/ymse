#!/usr/bin/perl
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker
#
# /proc/mdstat example {{{
#
# Personalities : [raid1] [raid6] [raid5] [raid4] [linear] [multipath] [raid0] [raid10] 
# md2 : active raid1 sde2[2] sdf[3]
#       244066304 blocks super 1.2 [2/2] [UU]
#       bitmap: 0/2 pages [0KB], 65536KB chunk
# 
# md1 : active raid1 sdd2[2] sda2[0]
#       976599424 blocks super 1.2 [2/2] [UU]
#       bitmap: 2/8 pages [8KB], 65536KB chunk
# 
# md0 : active raid6 sde1[9] sdh[7] sdg[10] sdd1[6] sdc[11] sdb[8]
#       1953021952 blocks super 1.2 level 6, 512k chunk, algorithm 2 [6/6] [UUUUUU]
#       [=====>...............]  resync = 26.6% (130114128/488255488) finish=25728.4min speed=231K/sec
#       bitmap: 4/4 pages [16KB], 65536KB chunk
# 
# unused devices: <none>
# }}}

use strict;
use warnings;

sub feilfeilfeil {
    my $s = shift;
    $s = "Undefined error!" unless (defined($s));
    $s =~ s/[\n\r]+$//;
    print STDERR "$s\n";
    exit(1);
}

my $mdstat_fn = '/proc/mdstat';
open my $mdstat,"$mdstat_fn" || die "Can't open $mdstat_fn for reading: $!\n";

my $md = shift;
&feilfeilfeil("Need md device in syntax of 'md0' or similar (not with /dev/)") if (!defined($md) or ($md =~ /^\//));
&feilfeilfeil("Devince '$md' is not a block device") unless (-b "/dev/$md");
&feilfeilfeil("Devicee '$md' isn't named like a block device") unless ($md =~ /md/);

my $found = 0;
my ($md_devs,$md_blocks);
while (!$found and my $line = <$mdstat>) {
    if ($line =~ /^$md/) {
        $md_devs = $line;
        $md_blocks = <$mdstat>;
        $found = 1;
    }
}

&feilfeilfeil("Wierd - $md is a block device, but doesn't show up in $mdstat_fn") unless ($found);

chomp($md_devs);
my @devfields = split(' ', $md_devs);

for (my $i=4;$i<=$#devfields;$i++) {
    my $dev=$devfields[$i];
    $dev =~ s/\[\d+\]//;
    print "$dev ";
}
print "\n";
exit(0);
