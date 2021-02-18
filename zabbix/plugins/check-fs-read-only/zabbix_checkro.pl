#!/usr/bin/perl
# vim:ts=4:sw=4:sts=4:ts:ai

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
	my $s = undef;
	local @ARGV = $_;

	if ($#ARGV > 1) {
		$s  = $ARGV[1];
	} elsif ($#ARGV > 2) {
		printf STDERR "Internal error: help() should be called with none or one argument\n";
		exit 2;
	}
	my $fst = "";
	$fst .= "$_," for (keys %fstypes);
	$fst =~ s/,$//;
	print "Syntax: $s\n" if (defined($s));
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

if ($^O eq "linux") {
	# /dev/sda2 /boot/efi vfat rw,relatime,fmask=0022,dmask=0022,codepage=437,iocharset=iso8859-1,shortname=mixed,errors=remount-ro 0 0
	open my $mounts,"/proc/mounts" || exit 1;
	while (my $line = <$mounts>) {
		my ($dev,$mp,$fst,$opts) = split(/\s+/, $line);
		next unless ($fstypes{$fst});
		my @optarr = split(/,/,$opts);
		push @rofilesystems, "$dev on $mp" if (grep $_ eq "ro", @optarr );
		print "Checking $fst filesystem $dev mounted on $mp\n" if (defined($verbose_a));
		$fscount++;
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
