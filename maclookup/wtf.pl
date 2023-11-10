#!/usr/bin/perl
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker

use strict;
use warnings;

my $maskinliste = "roysk.txt";
my $ouiliste = "oui.txt";
my $args = @ARGV;
my $liste = undef;
my @macs = ('52:54:00:bb:25:6e','52:54:00:58:a1:d1');

# if ($args > 0 and $ARGV[0] eq '-') {
#     print("Read from stdin...\n");
#     $liste = *STDIN;
#     # || die ("wtf? $!\n");
# } else {
#     print("open $maskinliste...\n");
#     open($liste, $maskinliste) || die ("wtf? $!\n");
# }
# 
# while (my $line = <$liste>) {
#     chomp($line);
#     $line = lc($line);
# }

sub mac_prefix {
    my $mac_adr = shift;
    my $separator = shift;

    $separator = ':' unless(defined($separator));
    my @mac_arr = split(/[\:\-]/, $mac_adr);
    my $prefix = join($separator, @mac_arr[0..2]);
    return $prefix;
}

foreach my $mac (@macs) {
    my $prefix = mac_prefix($mac, '-');
    print("Mac is $mac, prefix is $prefix\n");
}
#   print(join(';', @tull[0..3]) . "\n");
