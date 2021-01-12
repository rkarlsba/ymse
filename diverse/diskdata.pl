#!/usr/bin/perl
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker
# 
# Docs {{{
#
# Personalities : [raid1] [raid6] [raid5] [raid4] [linear] [multipath] [raid0] [raid10] 
# md0 : active raid6 sdh[12] sdk[11] sdg[10] sdm[13] sdl[14] sdi[15] sdj[19] sdn[20] sdd[16] sdc[17] sda[18](S) sdb[21](S)
#       15627067392 blocks super 1.2 level 6, 512k chunk, algorithm 2 [10/10] [UUUUUUUUUU]
#       bitmap: 7/15 pages [28KB], 65536KB chunk
# 
# md1 : active raid1 sdf2[1] sde2[0]
#       116171136 blocks [2/2] [UU]
#       bitmap: 1/1 pages [4KB], 65536KB chunk
# 
# unused devices: <none>
#
# }}}

use strict;
use warnings;
use Getopt::Long;
use Disk::SMART;

my $mddev = undef;
my $verbose=0;
my $mdstat_f = "/proc/mdstat";
my $mdstat_fn = undef;
my $nosmart = 0;

my %disks;

GetOptions(
    "d=s"       => \$mddev,
    "m=s"       => \$mdstat_fn,
    "n"         => \$nosmart,
    "verbose+"  => \$verbose,
) or die("Error in command line arguments\n");

if (defined($mdstat_fn)) {
    $mdstat_f = $mdstat_fn;
    unless ($nosmart) {
        print STDERR "Disabling SMART lookups since input isn't from local mdstat\n";
        $nosmart++;
    }
}

die("Need raiddev (-d)") unless (defined($mddev));
die("Remove /dev, please") if ($mddev =~ /^\/dev/);

# Lese diskinfo inn i hash
open(my $mdstat, "<", $mdstat_f) || die "Can't read mdstat file '$mdstat_f': $!";
while (my $line = <$mdstat>) {
    chomp($line);
    next unless ($line =~ /^$mddev/);
    my @parts = split(/\s+/, $line);
    foreach my $dev (@parts) {
        next unless ($dev =~ /^sd/);
        if ($dev =~ /(sd[a-z][a-z]?)\[(\d+)\](\((\w+)\))?/) {
            $disks{$1} = $4;
        }
    }
}

# Gå gjennom hashen
foreach my $disk (sort keys %disks) {
    if (defined($disks{$disk})) {
        if ($disks{$disk} eq "S") {
            print STDERR "Ignorerer $disk (spare)\n";
            next;
        } elsif ($disks{$disk} eq "F") {
            print STDERR "Ignorerer $disk (havarert)\n";
            next;
        }
    }
    print("$disk\n");
}
