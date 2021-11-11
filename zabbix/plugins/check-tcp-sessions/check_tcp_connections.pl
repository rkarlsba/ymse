#!/usr/bin/perl
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker

use strict;
use warnings;
use Getopt::Long;
#use Getopt::Mixed;

my $dummy = 0;
my $debug = 0;
my $port = undef;
my $total = 0;
my $count_conns = 0;
my $count_ips = 0;
my $count = 0;

my %ips;

# my $ss_tn {{{
my $ss_tn = <<EOT;
State                 Recv-Q                  Send-Q                                               Local Address:Port                                                    Peer Address:Port                  Process
ESTAB                 0                       0                                                   192.168.10.254:22                                                    84.213.115.180:58070
ESTAB                 0                       0                                                   192.168.10.254:22                                                    84.213.115.180:33122
ESTAB                 0                       0                                                   192.168.10.254:2049                                                   192.168.10.38:959
ESTAB                 0                       0                                                   192.168.10.254:22                                                    84.213.115.180:59976
ESTAB                 0                       0                                                   192.168.10.254:22                                                    84.213.115.180:45478
ESTAB                 0                       0                                                   192.168.10.254:22                                                    84.213.115.180:47956
ESTAB                 0                       0                                                        127.0.0.1:5011                                                       127.0.0.1:49400
ESTAB                 0                       0                                                        127.0.0.1:49400                                                      127.0.0.1:5011
ESTAB                 0                       0                                        [2a01:79c:cebf:61e4:1::1]:2049                                    [2a01:79c:cebf:61e4:300::41]:914
ESTAB                 0                       0                                        [2a01:79c:cebf:61e4:1::1]:2049                                    [2a01:79c:cebf:61e4:300::64]:934
ESTAB                 0                       0                                        [2a01:79c:cebf:61e4:1::1]:22                                        [2a01:79c:cebf:61e4:10::1]:60376
ESTAB                 0                       0                                        [2a01:79c:cebf:61e4:1::1]:22                                            [2001:700:700:15::162]:57692
ESTAB                 0                       0                                        [2a01:79c:cebf:61e4:1::1]:54480                                       [2a0a:51c0:0:1f:67a7::2]:22
ESTAB                 0                       0                                        [2a01:79c:cebf:61e4:1::1]:2049                                    [2a01:79c:cebf:61e4:300::77]:669
ESTAB                 0                       0                                        [2a01:79c:cebf:61e4:1::1]:22                                        [2a01:79c:cebf:61e4:10::1]:60378
ESTAB                 0                       0                                        [2a01:79c:cebf:61e4:1::1]:22                                          [2001:700:700:403::109b]:63130
ESTAB                 0                       0                                        [2a01:79c:cebf:61e4:1::1]:8822                                      [2001:700:700:403::c:107c]:57305
ESTAB                 0                       0                                        [2a01:79c:cebf:61e4:1::1]:8823                                      [2001:700:700:403::8:10d1]:55007
ESTAB                 0                       0                                        [2a01:79c:cebf:61e4:1::1]:22                                        [2a01:79c:cebf:61e4:10::1]:60374
ESTAB                 0                       0                                        [2a01:79c:cebf:61e4:1::1]:2049                                    [2a01:79c:cebf:61e4:300::84]:667"
EOT
# }}}

my $ss;
if ($^O eq "darwin") {
    $dummy = 1;
}

GetOptions(
    'h' => sub { &help },   'help'   => sub { &help },
    'd' => \$dummy,         'dummy' => \$dummy,
    'i' => \$count_ips,     'ips' => \$count_ips,
    't' => \$count_conns,   'total' => \$count_conns,
    'D+' => \$debug,        'debug+' => \$debug,
    'p=i' => \$port,        'port=i' => \$port,  
) || die 'wtf?';

if (defined($port)) {
    die "Port must be numeric\n" unless ($port =~ /^\d+$/);
    die "Port must be in the range of 1-65535\n" unless ($port >= 1 and $port <= 65535);
}

die "Please - either -i or -t\n" if ($count_ips and $count_conns);

if ($dummy gt 0) {
    open $ss,"ss.txt" || die "¿Qué?";
} else {
    print "ok..., kjør \"ss -tn|\"\n" if ($debug gt 1);
    open $ss,"ss -tn|" || die "Funker ikke!";
}

while (my $line = <$ss>) {
    my @cols = split(/\s+/,$line);
    next unless ($cols[0] eq "ESTAB");
    if (defined($port)) {
        next unless ($cols[3] =~ m/:$port$/);
    }
    print "DEBUG: $line\n" if ($debug);
    my $ip = $1 if ($cols[4] =~ /\[(.*?)\]/);
    unless (defined($ip)) {
        if ($cols[4] =~ /^(\d+\.\d+\.\d+\.\d+)\:/) {
            $ip = $1;
        }
    }
    if (defined($ip)) {
        # Vaske IPV4-adresser - ::ffff:10.252.3.19 til 10.252.3.19
        $ip =~ s/^::ffff://;
        print "$ip\n" unless ($count_conns || $count_ips);
        $ips{$ip}++;
        $count++;
    }
}

close $ss;

if ($count_conns) {
    print "$count\n";
} elsif ($count_ips) {
    my $ip_count = scalar keys %ips;
    print "$ip_count\n";
}
