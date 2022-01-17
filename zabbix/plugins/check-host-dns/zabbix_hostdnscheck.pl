#!/usr/bin/perl
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker

use Getopt::Long;
use POSIX qw(uname);

sub help {
    print<<EOT;
Usage: zabbix_hostdnscheck.pl < --host hostname > < --ip ip-address > [ --help ]

    --help        This helptext

    --host        The hostname from zabbix
    --ip          The IP address from zabbix

EOT
    exit(0);
}

# usikker på om jeg trenger denne {{{
# unless ($supported_OSes{$^O}) {
#     print STDERR "Operating system '$^O' not supported\n";
#     exit(1);
# }
# }}}

GetOptions(
    'help'       => sub { &help },
    'host'       => \$opts{host},
    'ip'         => \$opts{ip},
);


