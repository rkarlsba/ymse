#!/usr/bin/env perl
# vim:ts=4:sw=4:sts=4:et:ai

use strict;
use warnings;
use Getopt::Long;
use POSIX qw(uname);
use Data::Validate::IP;

# Opts
my $opt_host;
my $opt_port = 443;
my $opt_help;
my $opt_verbose;
my $daredevil = 1;

my $nmap = '/opt/nmap/bin/nmap';

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
    --port | -p <n>     Port to check ($opt_port)

EOT
    exit($exitcode);
}

Getopt::Long::Configure('bundling');
GetOptions(
    "verbose+"       => \$opt_verbose,  "v+" => \$opt_verbose,
    "port=i"         => \$opt_port,     "p=i" => \$opt_port,
    "help"           => \$opt_help,     "h" => \$opt_help,
) or die "Invalid argument";

if (($< == 0 or $> == 0) and not $daredevil) {
    print STDERR "Won't run as root\n";
    exit(2);
}

$opt_host = shift;
&help if ($opt_help);
&help("Need host") unless (defined($opt_host));

# Input validation
my $ipv6 = "";
if ($opt_host =~ /\\\//) {
    print STDERR "Invalid characters in hostname\n";
    exit(3);
}
if ($opt_host =~ /^[a-z0-9\-]+(\.[a-z0-9\-]+(\.[a-z0-9\-]+)?)?/i) {
    print "Valid hostname \$opt_host\n" if ($opt_verbose);
} elsif($opt_host =~ /^
(
([0-9a-fA-F]{1,4}:){7,7}[0-9a-fA-F]{1,4}|          # 1:2:3:4:5:6:7:8
([0-9a-fA-F]{1,4}:){1,7}:|                         # 1::                              1:2:3:4:5:6:7::
([0-9a-fA-F]{1,4}:){1,6}:[0-9a-fA-F]{1,4}|         # 1::8             1:2:3:4:5:6::8  1:2:3:4:5:6::8
([0-9a-fA-F]{1,4}:){1,5}(:[0-9a-fA-F]{1,4}){1,2}|  # 1::7:8           1:2:3:4:5::7:8  1:2:3:4:5::8
([0-9a-fA-F]{1,4}:){1,4}(:[0-9a-fA-F]{1,4}){1,3}|  # 1::6:7:8         1:2:3:4::6:7:8  1:2:3:4::8
([0-9a-fA-F]{1,4}:){1,3}(:[0-9a-fA-F]{1,4}){1,4}|  # 1::5:6:7:8       1:2:3::5:6:7:8  1:2:3::8
([0-9a-fA-F]{1,4}:){1,2}(:[0-9a-fA-F]{1,4}){1,5}|  # 1::4:5:6:7:8     1:2::4:5:6:7:8  1:2::8
[0-9a-fA-F]{1,4}:((:[0-9a-fA-F]{1,4}){1,6})|       # 1::3:4:5:6:7:8   1::3:4:5:6:7:8  1::8  
:((:[0-9a-fA-F]{1,4}){1,7}|:)|                     # ::2:3:4:5:6:7:8  ::2:3:4:5:6:7:8 ::8       ::     
fe80:(:[0-9a-fA-F]{0,4}){0,4}%[0-9a-zA-Z]{1,}|     # fe80::7:8%eth0   fe80::7:8%1     (link-local IPv6 addresses with zone index)
::(ffff(:0{1,4}){0,1}:){0,1}
((25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9])\.){3,3}
(25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9])|          # ::255.255.255.255   ::ffff:255.255.255.255  ::ffff:0:255.255.255.255  (IPv4-mapped IPv6 addresses and IPv4-translated addresses)
([0-9a-fA-F]{1,4}:){1,4}:
((25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9])\.){3,3}
(25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9])           # 2001:db8:3:4::192.0.2.33  64:ff9b::192.0.2.33 (IPv4-Embedded IPv6 Address)
)
/) {
    print "Valid IPv6 address \$opt_host\n" if ($opt_verbose);
    $ipv6 = "-6";
} elsif ($opt_host =~ /^\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}$/) {
    print "Valid IPv4 address \$opt_host\n" if ($opt_verbose);
} else {
    print "Invalid hostname \"$opt_host\"\n";
    exit(4);
}
# nmap -sV --script ssl-enum-ciphers -p 443
#my $nmap_cmd = "$nmap -sV --script ssl-enum-ciphers -p $opt_port $opt_host";
my $nmap_cmd = "$nmap $ipv6 --script ssl-enum-ciphers -p $opt_port $opt_host";
open(my $nmap_output, "$nmap_cmd|") ||
    die("Can't run nmap command \"$nmap_cmd\"\n");

while (my $line = <$nmap_output>) {
    print $line;
}
