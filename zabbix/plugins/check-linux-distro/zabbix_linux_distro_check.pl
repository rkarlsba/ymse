#!/usr/bin/perl
# vim:ts=4:sw=4:sws=4:et:fdm=marker

use Getopt::Long;
use POSIX qw(uname);

# Variables
my %supported_OSes = (
    'linux' => 1,
);
my %opts = (
    name => 1,
);
#my %opts;
my $distro = '*unknown*';
my $hrdistro = undef; # Human readable distro name with caps and all
my $distvers = undef;
my @uname = uname;
my $arch = $uname[4];
my ($disf,$s);

sub help {
    print<<EOT;
Usage: zabbix_linux_distro_check.pl [--help | --distvers | --arch | --friendly ]

    --help        This helptext

These options are all mutually exclusive.

    --name        Report distro name
    --vers        Report the (major) distro version
    --arch        Report system architecture (x86, arm etc)
    --friendly    Repot a full, friendly, human readable version with everything(?) relevant

Extras

    --scientific  Treat Scientific Linux as a separate distro and not just CentOS

EOT
    exit(0);
}

unless ($supported_OSes{$^O}) {
    print STDERR "Operating system '$^O' not supported\n";
    exit(1);
}

GetOptions(
    'help'       => sub { &help },
    'name'       => \$opts{name},
    'vers'       => \$opts{vers},
    'arch'       => \$opts{arch},
    'full|f'     => \$opts{friendly},
    'friendly'   => \$opts{friendly},

    'scientific' => \$opts{scientific},
);

# New code as of 2019-06-12 by Roy Sigurd Karlsbakk <roysk@oslomet.no>
if ( -r '/etc/os-release' ) {
    open $disf,'/etc/os-release' || die "File /etc/os-release should be readable, but I cound't open it $!\n";
    while ($s = <$disf>) {
        if ($s =~ /^ID=(.*$)/) {
            $distro = lc($1);
            next;
        } elsif ($s =~ /^VERSION_ID=(.*$)/) {
            $distvers = $1;
            next;
        }
    }
    close $disf;
} elsif ( -r '/etc/redhat-release' ) {
# Red Hat Enterprise Linux Server release 7.6 (Maipo)
# Red Hat Enterprise Linux Server release 6.10 (Santiago)
# CentOS Linux release 7.6.1810 (Core)
    open $disf,'/etc/redhat-release' || die "File /etc/lsb-release should be readable, but I cound't open it $!\n";
    while ($s = <$disf>) {
        if ($s =~ /^(red hat).*?release (\d+\.\d+)/i) {
            $distro = 'rhel';
            $distvers = $1;
            next;
        } elsif ($s =~ /^centos.*?release (\d+\.\d+)/i) {
            $distro = 'centos';
            $distvers = $1;
            next;
        } elsif ($s =~ /^Scientific.*?release (\d+\.\d+)/i) {
            if ($opts{scientific}) {
                $distro = 'scientific';
                $distvers = $1;
            } else {
                $distro = 'centos';
                $distvers = $1;
            }
            next;
        }
    }
    close $disf;
} elsif ( -r '/etc/lsb-release' ) {
    open $disf,'/etc/lsb-release' || die "File /etc/lsb-release should be readable, but I cound't open it $!\n";
    while ($s = <$disf>) {
        if ($s =~ /^DISTRIB_ID=(.*$)/) {
            $distro = lc($1);
            next;
        } elsif ($s =~ /^DISTRIB_RELEASE=(.*$)/) {
            $distvers = $1;
            next;
        }
    }
    close $disf;
}
# Vaske litt
$distro =~ s/["']//g;
$distvers =~ s/["']//g;

if ($opts{vers}) {
    print "$distvers\n";
} elsif ($opts{arch}) {
    print ("$arch\n");
} elsif ($opts{friendly}) {
    if ($distro eq "centos") {
        $hrdistro = "CentOS Linux";
    } elsif ($distro eq "scientific") {
        $hrdistro = "Scientific Linux";
    } elsif ($distro eq "rhel") {
        $hrdistro = "RHEL";
    } elsif ($distro eq "ubuntu") {
        $hrdistro = "Ubuntu";
    } elsif ($distro eq "raspbian") {
        $hrdistro = "Raspbian GNU/Linux";
    } elsif ($distro eq "debian") {
        $hrdistro = "Debian GNU/Linux";
    } else {
        $hrdistro = "Cannot find distro \"$distro\"";
    }
    print("$hrdistro $distvers ($arch)\n");
} elsif ($opts{name}) {
    print "$distro\n";
} else {
    print("The pilot is quitely drunk today. We apologize for the inconvencienceice\n");
    exit(1);
}

exit 0;
