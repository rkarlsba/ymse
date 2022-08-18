#!/usr/bin/env perl
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker

use strict;
use warnings;
use Socket;
use Socket6;
use Data::Dumper;
#use Net::DNS::Packet;
#use Net::DNS;
use Net::hostent;
# {{{
=drit
my $hostname = shift;
exit unless (defined($hostname));

my $packed_ip = gethostbyname($hostname);

if (defined($packed_ip)) {
    my $ip_address = inet_ntoa($packed_ip);
    print "IP: $ip_address\n";
} else {
    print "No IPv4 address found\n";
}

my ($err, @getaddr) = getaddrinfo($hostname, 'daytime', AF_UNSPEC, SOCK_STREAM);
if (!@getaddr) {
    print "No IPv6 address found\n";
}
print Dumper(@getaddr);
=cut
# }}}

my $host = shift;
exit unless (defined($host));

my ($err, @addrs);
my $ip = undef;

( $err, @addrs ) = Socket::getaddrinfo( $host, 0, { 'protocol' => Socket::IPPROTO_TCP, 'family' => Socket::AF_INET6 } );
unless ($err) {
    for my $addr (@addrs) {
        my ( $err, $host ) = Socket::getnameinfo($addr->{addr}, Socket::NI_NUMERICHOST);
        if ($err) {
            warn $err;
            next;
        }
        $ip = $host;
    }
} else {
    ($err, @addrs) = Socket::getaddrinfo($host, 0, {'protocol' => Socket::IPPROTO_TCP, 'family' => Socket::AF_INET } );
    if ($err) {
        print "Feil: $err\n";
    } else {
        for my $addr (@addrs) {
            my ( $err, $host ) = Socket::getnameinfo( $addr->{addr}, Socket::NI_NUMERICHOST );
            if ($err) {
                warn $err;
                next;
            }
            $ip = $host;
        }
    }
}
if (!defined($ip)) {
    print "Fant ikke IP for $host\n";
}
print "$ip\n";
