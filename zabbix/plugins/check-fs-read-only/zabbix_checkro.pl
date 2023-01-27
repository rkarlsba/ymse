#!/usr/bin/perl
# vim:ts=4:sw=4:sts=4:ts:ai:fdm=marker

###############################################################################
# checkro.pl by Roy Sigurd Karlsbakk <roy@karlsbakk.net>
# Version 0.1
###############################################################################

use strict;
use warnings;
use Getopt::Long;

# Hash with mountpoints that should be ignored, by mountpoint, default none, as
# %fstypes below usually suffices.
my %ignoremp = ();

# By default, include most common data filesystem types
my %fstypes = (
	"ext2" => 1,
	"ext3" => 1,
	"ext4" => 1,
	"jfs" => 1,
	"xfs" => 1,
	"zfs" => 1,
	"hfs" => 1,
	"vfat" => 1,
	"msdos" => 1,
);

# Give up if we're running on other OSes than these
my %supported_oses = (
	'linux' => 1,
	'darwin' => 1,
);

# Globals
my @rofilesystems = undef;
my $fscount = 0;

# Argument variables
my $fstypes_a = undef;
my $help_a = undef;
my $ignoremp_a = undef;
my $verbose_a = undef;

# subs
sub syntax {
	my $s = shift;
	chomp($s);

	my $fst = "";
	$fst .= "$_," for (keys %fstypes);
	$fst =~ s/,$//;
	print "Error: $s\n" if (defined($s));
	print "$0 <args>\n";
	print " [ --fstypes fs1,fs2,... ]     # Include these filesystem types\n";
	print "    Currently set to '$fst'\n";
	print " [ --ignoremp mp1,mp2,... ]    # Ignore these mountpoints (not implemented yet)\n";
	print " -v --verbose                  # Be verbose\n";
	exit 1;
}

# OS check
&syntax("Unsupported operating system $^O\n") unless ($supported_oses{$^O});

# Check options
Getopt::Long::Configure('bundling');
GetOptions(
    "verbose"       => \$verbose_a,     "v" => \$verbose_a,
    "help"          => \$help_a,        "h" => \$help_a,
    "fstypes=s"     => \$fstypes_a,
    "ignoremp=s"    => \$ignoremp_a,
) or die "Invalid argument";

if (defined($fstypes_a)) {
	undef %fstypes;
	foreach (split(/,/, $fstypes_a)) {
		$fstypes{$_} = 1;
	}
}
&syntax if (defined($help_a));

my ($dev,$mp,$fst,$opts);

if ($^O eq "linux") {
	# /dev/sda2 /boot/efi vfat rw,relatime,fmask=0022,dmask=0022,codepage=437,iocharset=iso8859-1,shortname=mixed,errors=remount-ro 0 0
	open my $mounts,"/proc/mounts" || exit 1;
	while (my $line = <$mounts>) {
		($dev,$mp,$fst,$opts) = split(/\s+/, $line);
		next unless ($fstypes{$fst});
		next if ($mp =~ /^\/var\/snap\//);
		my @optarr = split(/,/,$opts);
		push @rofilesystems, "$dev on $mp" if (grep $_ eq "ro", @optarr );
		print "Checking $fst filesystem $dev mounted on $mp\n" if (defined($verbose_a));
		$fscount++;
	}
} elsif ($^O eq "darwin") {
	open my $mounts,"mount|" || exit 1;
	while (my $line = <$mounts>) {
		chomp($line);
		# /dev/disk1s1 on / (apfs, local, read-only, journaled)
		# devfs on /dev (devfs, local, nobrowse) {{{
		# /dev/disk1s2 on /System/Volumes/Data (apfs, local, journaled, nobrowse)
		# /dev/disk1s5 on /private/var/vm (apfs, local, journaled, nobrowse)
		# map auto_home on /System/Volumes/Data/home (autofs, automounted, nobrowse)
		# /dev/disk1s4 on /Volumes/Recovery (apfs, local, journaled, nobrowse)
		# /dev/disk2s1 on /Volumes/PrusaSlicer (hfs, local, nodev, nosuid, read-only, noowners, quarantine, mounted by roy)
		#
		# if ($line =~ m/(^[a-z0-9\/]+)\s+on\s+\(\w+)\s+(.*?)/) {
		# }}}
		if ($line =~ m/(^[a-zA-Z0-9\/]+|map auto_home)\son\s([a-zA-Z0-9\/]+)\s\((\w+), (.*?)\)/) {
			print "mp er $mp\n";
			foreach my $opt (split(/,\s*/, $opts,)) {
				if ($opt eq "read-only") {
					$fscount++;
					push @rofilesystems, "$dev on $mp";
				}
			}
			#print "$dev on $mp is $fst with opts $opts\n";
		} else {
			print "This one failed the regex: $line\n";
		}
	}
} else {
	print STDERR "WTF?\n";
	exit 3;
}

if ($#rofilesystems > 0) {
	printf("WARNING %d of %d filesystems mounted read-only: ", $#rofilesystems, $fscount);
	foreach (my $i=1;$i<$#rofilesystems;$i++) {
		print "$rofilesystems[$i], ";
	}
	print "$rofilesystems[$#rofilesystems]\n";
	exit 1;
}

if ($fscount > 0) {
	print "OK All $fscount filesystems checked are mounted read-write.\n";
} else {
	print "NOTICE No filesystems checked for being mounted read-write.\n";
}
exit 0;
