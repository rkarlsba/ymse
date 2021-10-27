#!/usr/bin/perl
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker

use strict;
use warnings;
use Getopt::Long;

# my ss_tn {{{
my $ss_tn =
"State                 Recv-Q                  Send-Q                                               Local Address:Port                                                    Peer Address:Port                  Process
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
# }}}
# Prat {{{
=begin

Tell antall sesjoner åpent mot Squid

# ss -tn
ESTAB 0      0        [2001:700:700:1::89]:3128  [2001:700:700:200b::12]:34801
ESTAB 0      0        [2001:700:700:1::89]:3128  [2001:700:702:2513::37]:58753
ESTAB 0      0        [2001:700:700:1::89]:3128  [2001:700:700:200b::13]:58177
ESTAB 0      0      [::ffff:158.36.161.89]:3128     [::ffff:10.252.3.19]:60567
ESTAB 0      0        [2001:700:700:1::89]:3128   [2001:700:700:25::320]:56486
ESTAB 0      0        [2001:700:700:1::89]:3128   [2001:700:700:25::320]:52272
ESTAB 0      0      [::ffff:158.36.161.89]:3128     [::ffff:10.252.3.19]:60566

=cut
# }}}}

my $ss;
if ($^O eq "Darwin") {
    ZZ
    open my $ss,"ss -tn|" || die "Funker ikke!";

while (my $line = <$ss>) {
    my @cols = split(/\s+/,$line);
    next unless ($cols[0] eq "ESTAB");
    next unless ($cols[3] =~ m/:3128$/);
    my $ip = $1 if ($cols[4] =~ /\[(.*?)\]/);
    # Vaske IPV4-adresser - ::ffff:10.252.3.19 til 10.252.3.19
    $ip =~ s/^::ffff://;
    print "$ip\n";
}

close $ss;
