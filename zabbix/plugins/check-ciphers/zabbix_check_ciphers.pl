#!/usr/bin/env perl
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker

use strict;
use warnings;
use Getopt::Long;
use POSIX qw(uname);
use Socket;

my $have_data_validate_ip = eval {
  require Data::Validate::IP;
  Data::Validate::IP->import();
  1;
};

my $have_data_validate_domain = eval {
  require Data::Validate::Domain;
  Data::Validate::Domain->import();
  1;
};

# Globals
my %goodcerts = (
    'TLSv1.2' => 1,
    'TLSv1.3' => 1,
);
my %badcerts = (
    'SSLv1' => 1,
    'SSLv2' => 1,
    'SSLv3' => 1,
    'TLSv1.0' => 1,
    'TLSv1.1' => 1,
);

# Opts
my $opt_help;
my $opt_host;
my $opt_ipv4;
my $opt_ipv6;
my $opt_nmap_path = "nmap";
my $opt_port = 443;
my $opt_quiet;
my $opt_sudo;
my $opt_verbose;
my $daredevil = 1;
my $ip;

# Egenkompilert nmap - den som ligger her fra før er hønngammal
my $custom_nmap = '/opt/nmap/bin/nmap';
my $nmap;

sub help {
    my $exitcode = 0;
    my $s = shift;
    if (defined($s)) {
        print "Error: " . $s . "\n";
        $exitcode=1;
    }
    print<<EOT;
Usage: zabbix_check_ciphers.pl [--help | --port <n> ] host

    --help              This helptext
    --verbose | -v      Don't be brief or distinct, but rather spend the day explaining what may be going wrong or right and make sure you tell the user/admin every detail available because otherwise she or he might be unsure of what to do and will be walking around in distress and probably become a drug addict or worse before the day is over.
    --quiet | -q        STFU
    --port | -p <n>     Port to check ($opt_port)
    --nmap-path | -N    Path to nmap if not in the current path
    -4                  Use IPv4 (automatic if Data::Validate::IP is installed)
    -6                  Use IPv6 (automatic if Data::Validate::IP is installed)

EOT
    exit($exitcode);
}

Getopt::Long::Configure('bundling');
GetOptions(
    "verbose+"      => \$opt_verbose,   "v+" => \$opt_verbose,
    "quiet+"        => \$opt_quiet,     "q" => \$opt_quiet,
    "port=i"        => \$opt_port,      "p=i" => \$opt_port,
    "nmap-path=s"   => \$opt_nmap_path, "N=s" => \$opt_nmap_path,
    "sudo"          => \$opt_sudo,      "S" => \$opt_sudo,
    "6"             => \$opt_ipv6,
    "4"             => \$opt_ipv4,
    "help"          => \$opt_help,      "h" => \$opt_help,
) or die "Invalid argument";

if (($< == 0 or $> == 0) and not $daredevil) {
    print STDERR "Won't run as root\n";
    exit(2);
}

$opt_host = shift;
&help if ($opt_help);
&help("Need host") unless (defined($opt_host));
&help("Can't use both IPv4 and IPv6") if (defined($opt_ipv4) and defined($opt_ipv6));

# Input validation
my $ipv6 = undef;
my $valid_ip = 0;

if ($opt_ipv4) {
    my $ipv6 = "";
} elsif ($opt_ipv6) {
    $ipv6 = "-6";
}


if (!defined($ipv6)) {
    if ($have_data_validate_ip) {
        print "opt_host er $opt_host\n" if (defined($opt_verbose) and ($opt_verbose > 1));
        if (is_ipv4($opt_host)) {
            $valid_ip++;
            print "Valid IPv4 address \"$opt_host\" [1]\n" if ($opt_verbose);
            $ipv6 = "";
        } elsif (is_ipv6($opt_host)) {
            $valid_ip++;
            print "Valid IPv6 address \"$opt_host\" [1]\n" if ($opt_verbose);
            $ipv6 = "-6";
        }
    }
} else {
    print STDERR "Library Data::Validate::IP not found - use -4 or -6\n" unless ($opt_quiet or $opt_ipv4 or $opt_ipv6);
}

unless ($valid_ip) {
    if ($have_data_validate_domain) {
        if (is_domain($opt_host)) {
            print "Valid hostname \"$opt_host\"\n" if ($opt_verbose);
            # Fine, but we don't know if it's IPv4 or IPV6
            my ( $err, @addrs ) = Socket::getaddrinfo( $opt_host, 0, { 'protocol' => Socket::IPPROTO_TCP, 'family' => Socket::AF_INET6 } );
            unless ($err) {
                for my $addr (@addrs) {
                    my ( $err, $opt_host ) = Socket::getnameinfo($addr->{addr}, Socket::NI_NUMERICHOST);
                    if ($err) {
                        warn $err;
                        next;
                    }
                    $ip = $opt_host;
                    $ipv6 = "-6";
                }
            } else {
                ($err, @addrs) = Socket::getaddrinfo($opt_host, 0, {'protocol' => Socket::IPPROTO_TCP, 'family' => Socket::AF_INET } );
                if ($err) {
                    print "Feil: $err\n";
                } else {
                    for my $addr (@addrs) {
                        my ( $err, $opt_host ) = Socket::getnameinfo( $addr->{addr}, Socket::NI_NUMERICHOST );
                        if ($err) {
                            warn $err;
                            next;
                        }
                        $ip = $opt_host;
                    }
                }
            }
            if (!defined($ip)) {
                print "Fant ikke IP for $opt_host\n";
            }
        } else {
            print "Invalid hostname \"$opt_host\"\n";
            exit(4);
        }
    } else {
        print STDERR "Library Data::Validate::Domain not found\n" unless ($opt_quiet);
    }
}

# nmap-greier {{{
#
# nmap -sV --script ssl-enum-ciphers -p 443
# Trenger nok ikke -sV - fra manualen:
#   Probe open ports to determine service/version info
#
# my $nmap_cmd = "$nmap $ipv6 -sV --script ssl-enum-ciphers -p $opt_port $opt_host";
#
# }}}
$ipv6 = "" unless (defined($ipv6));

my $sudo = "";
$sudo = "sudo" if ($opt_sudo);

my $nmap_cmd = "$sudo $opt_nmap_path $ipv6 --script ssl-enum-ciphers -p $opt_port $opt_host";
print "$nmap_cmd\n" if (defined($opt_verbose) and $opt_verbose >= 2);
open(my $nmap_output, "$nmap_cmd|") ||
    die("Can't run nmap command \"$nmap_cmd\"\n");

=output {{{

PORT    STATE SERVICE
443/tcp open  https
| ssl-enum-ciphers:
|   TLSv1.0:
|     ciphers:
|       TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA (ecdh_x25519) - A
|       TLS_DHE_RSA_WITH_AES_256_CBC_SHA (dh 2048) - A

=cut }}}

my $tls;
my $warnmsg = undef;
my $errmsg = undef;
my @goodprotos = ();
my @badprotos = ();
my @unknownprotos = ();

while (my $line = <$nmap_output>) {
    if ($line =~ /^\|   ((TLS|SSL)v\d+.\d+):/) {
        $tls = $1;
        if ($goodcerts{$tls}) {
            push @goodprotos,$tls;
            print "YEY! $tls!\n" if ($opt_verbose);
        } elsif ($badcerts{$tls}) {
            push @badprotos,$tls;
            print "UH! $tls!\n" if ($opt_verbose);
        } else {
            push @unknownprotos,$tls;
            print "UH? $tls!\n" if ($opt_verbose);
        }
        next;
    } elsif ($line =~ /\(0 hosts up\)/) {
        print "ERROR: Host is down\n";
        exit(0);
    }
}

if ($#badprotos > 0) {
    $errmsg = "ERROR: Bad protocols: ";
    foreach my $p (@badprotos) {
        $errmsg .= "$p ";
    }
}

if ($#unknownprotos > 0) {
    $warnmsg = "WARNING: Unknown protocols: ";
    foreach my $p (@unknownprotos) {
        $warnmsg .= "$p ";
    }
}

if (defined($errmsg) || defined($warnmsg)) {
    print $errmsg if (defined($errmsg));
    print $warnmsg if (defined($warnmsg));
    print "\n";
} else {
    print "OK: Supported protocols: ";
    foreach my $p (@goodprotos) {
        print "$p ";
    }
    print "\n";
}
