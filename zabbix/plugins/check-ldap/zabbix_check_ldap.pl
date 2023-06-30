#!/usr/bin/env perl
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker
#
# zabbix_check_ldap.pl - do an LDAP query
#
# By Roy Sigurd Karlsbakk <roysk@oslomet.no>
# 
# Usage:
#
# zabbix_check_ldap.pl -b "dc=something,dc=tld" -U ldaps://ldap.something.tld
#

use strict;
use warnings;
use Getopt::Long;

# Globals
my $ldap_cmd_tmpl = 'ldapsearch -LLL -H %s -s base -b "%s" -x 2>&1';
my $verbose = 0;

# Opts
my $opt_basedn = undef;
my $opt_help = undef;
my $opt_ipv4 = undef;
my $opt_ipv6 = undef;
my $opt_quiet = undef;
my $opt_url = undef;
my $opt_verbose = undef;
my $opt_daredevil = undef;

sub help {
    my $exitcode = 0;
    my $s = shift;
    if (defined($s)) {
        print "Error: " . $s . "\n";
        $exitcode=1;
    }
    print<<EOT;
Usage: zabbix_check_ldap.pl [--help | --port <n> ] host

    --help              This helptext
    --verbose | -v      Don't be brief or distinct, but rather spend the day explaining what may be going wrong or right and make sure you tell the user/admin every detail available because otherwise she or he might be unsure of what to do and will be walking around in distress and probably become a drug addict or worse before the day is over.
    --quiet | -q        STFU
    --url               Give the URL to the server, something like ldaps://somewhere.tld
    --basedn            Give the base DN to search, something like dc=somewhere,dc=tld

EOT
    exit($exitcode);
}

Getopt::Long::Configure('bundling');
GetOptions(
    "verbose+"  => \$opt_verbose,   "v+" => \$opt_verbose,
    "quiet+"    => \$opt_quiet,     "q" => \$opt_quiet,
    "6"         => \$opt_ipv6,
    "4"         => \$opt_ipv4,
    "help"      => \$opt_help,
    "url=s"     => \$opt_url,       "u=s" => \$opt_url,
    "basedn=s"  => \$opt_basedn,    "b=s" => \$opt_basedn,
    "daredevil" => \$opt_daredevil,
) or die "Invalid argument";

&help if ($opt_help);
&help("Need base URL") unless (defined($opt_url));
&help("Need base Base DN") unless (defined($opt_basedn));
&help("Can't use both IPv4 and IPv6") if (defined($opt_ipv4) and defined($opt_ipv6));
warn("WARNING: IP stack choice (-4/-6) not implemented\n") if (defined($opt_ipv4) or defined($opt_ipv6));
$verbose=$opt_verbose if (defined($opt_verbose));

if (($< == 0 or $> == 0) and not $opt_daredevil) {
    print STDERR "Won't run as root\n";
    exit(2);
}

my $ldap_dn = "";
my $ldap_dc = "";
my $ldap_mail = "";
my $ldap_cmd = sprintf("$ldap_cmd_tmpl", $opt_url, $opt_basedn);
my $ldap_dump = "";
my $check_status = "WTF?";
print("> $ldap_cmd\n") if ($verbose);
open(my $ldap_fd,"$ldap_cmd|");
if ($ldap_fd) {
    while (<$ldap_fd>) {
        chomp();
        if (/^dn: (.*?)$/) {
            $ldap_dn = $1;
        } elsif (/^dc: (.*?)$/) {
            $ldap_dc = $1;
        } elsif (/^mail: (.*?)$/) {
            $ldap_mail = $1;
        }
        printf("> %s\n", $_) if ($verbose);
        $ldap_dump .= $_ . " ";
    }
}
close($ldap_fd);
my $exitcode = ($? >> 8);
print "command returned $exitcode\n" if ($verbose);

if ($exitcode == 0) {
    $check_status = "OK: ldapsearch found DN: $ldap_dn, DC: $ldap_dc, Mail: $ldap_mail";
} else {
    $check_status = "WARNING [$exitcode]: $ldap_dump";
}
print "$check_status\n";

exit($exitcode);
