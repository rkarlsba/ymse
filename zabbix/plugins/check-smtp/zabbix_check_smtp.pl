#!/usr/bin/perl
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker

use strict;
use warnings;
use Net::SMTP;
use Getopt::Long;

# globals
my $o_hostname;
my $o_port = 25;
my $o_ssl = undef;
my $o_timeout = 5;
my $o_debug = 0;
my $o_hello = `hostname -f`;
chomp($o_hello);

sub help {
    my $exitcode = 0;
    my $errmsg = shift;
    if (defined($errmsg)) {
        chomp($errmsg);
        print("ERROR: $errmsg\n");
        $exitcode = 1;
    }
    print<<EOT;
Usage: zabbix_check_smtp.pl <-h host-name-or-IP> [-p port] [-s ssl|starttls ]

    --help        This text

These options are all mutually exclusive.

    --hostname -h Name or IP address of host to check
    --port -p     Port number [$o_port]
    --ssl -s      Use SSL/TLS [none]

    --hello       Hostname to use for greeting [$o_hello]
    --timeout -t  Timeout [$o_timeout]
    --debug -d    Debug [$o_debug]

EOT
    exit($exitcode);
}

GetOptions(
    'help'       => sub { &help },
    'hostname=s' => \$o_hostname,  'h=s' => \$o_hostname,
    'port=i'     => \$o_port,      'p=i' => \$o_port,
    'ssl=s'      => \$o_ssl,       's=s' => \$o_ssl,
);

&help("Need hostname") unless (defined($o_hostname));

my $smtp = Net::SMTP->new(
    $o_hostname,
    Port => $o_port,
    Timeout => $o_timeout,
    Debug => $o_debug,
    Hello => $o_hello,
);
unless ($smtp) {
    print STDERR "Error: ", $smtp->message();
    exit(1);
}
print $smtp->domain . "\n";
$smtp->quit;

