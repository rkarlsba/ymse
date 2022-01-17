#!/usr/bin/perl
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker

use strict;
use warnings;
use Getopt::Long;

# Dokumentasjon {{{
#
# Skriptet går gjennom /proc/mounts og ser etter filsystemer montert read-only,
# siden det er noe vi generelt ikke vil ha. For noen filsystemer og/eller
# monteringspunkter, er dette imidlertid vanlig,som i tilfellet /sys/fs/cgroup
# og av og til /boot/efi. Skriptet inneholder derfor variabler for dette slik at
# disse kan ekskluderes for å unngå falske positiver.
#
# Dette skriptet går gjennom alt som ikke er ekskludert av filsystemer og
# rapporterer read-only-status for filsystene i JSON-format som senere er fôra
# til zabbix. Dette gjør at zabbix lager en ny item for hvert av filsystemene
# slik at de rapporteres individuelt, altså en bedre og mer dynamisk variant av
# det gamle skriptet zabbix-check-ro.pl
#
# Roy Sigurd Karlsbakk <roysk@hioa.no> <roy@karlsbakk.net>
#
# }}}

my %ignored_fstypes = (
    'cgroup' => 0,
    'devtmpfs' => 0,
    'securityfs' => 0,
    'tmpfs' => 0,
);

my %ignored_dirs = (
    '/sys/fs/cgroup' => 1,
    '/boot/efi' => 1,
);

my $first = 1;
my $procmounts = '/proc/mounts';

print "{\n";
print "\t\"data\":[\n\n";

open my $pm,$procmounts || die "Can't open $procmounts: $!\n";

while (<$pm>) {
    # /dev/mapper/vgstudfelles-lvstudfelles /data ext4 rw,relatime,data=ordered,jqfmt=vfsv0,usrjquota=aquota.user 0 0
    my ($fsname, $fstype, $fsopts) = m/\S+ (\S+) (\S+) (\S+)/;
    next if ($ignored_fstypes{$fstype});
    next if ($ignored_dirs{$fsname});

    my @fsopts = split(/,/, $fsopts);
    my $fsro = 0;
    foreach my $fsopt (@fsopts) {
        if ($fsopt eq 'ro') {
            $fsro++;
            last;
        }
    }

    unless ($first) {
        print "\t,\n";
        $first = 0;
    }

    print "\t{\n";
    print "\t\t\"{#FSNAME}\":\"$fsname\",\n";
    print "\t\t\"{#FSTYPE}\":\"$fstype\"\n";
    print "\t\t\"{#FSRO}\":\"$fsro\"\n";
    print "\t}\n";
}
close $pm;

print "\n\t]\n";
print "}\n";

