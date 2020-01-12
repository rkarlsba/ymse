#!/usr/bin/perl
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker

use strict;
use warnings;
use Term::ANSIColor qw(:constants);

# Globals
my $debug = 2;

# Denne vil ha mat fra dmesg med tidsstempel. Her er et eksempel på noe fra dmesg -T. {{{
#
# Se dmesg-T.txt for mer.
#
# [Fri Jan 10 10:34:04 2020] [UFW BLOCK] IN=wlan0 OUT= MAC=b8:27:eb:19:f3:0f:fc:83:99:54:4a:95:08:00 SRC=172.17.21.60 DST=172.17.28.93 LEN=64 TOS=0x00 PREC=0x00 TTL=63 ID=51182 DF PROTO=TCP SPT=61522 DPT=8123 WINDOW=65535 RES=0x00 CWR ECE SYN URGP=0 
# [Sun Jan 12 01:50:48 2020] [UFW BLOCK] IN=wlan0 OUT= MAC=b8:27:eb:19:f3:0f:a8:66:7f:1c:cf:7c:08:00 SRC=172.17.28.48 DST=172.17.28.93 LEN=40 TOS=0x00 PREC=0x00 TTL=64 ID=32616 PROTO=TCP SPT=40561 DPT=9100 WINDOW=65535 RES=0x00 SYN URGP=0 
#
# }}}

while (my $line = <STDIN>) {
    chomp($line);
    if ($line =~ /^\[(.*?)\] (\[UFW.*?\]) (.*)/) {
        my %packet = (
            IN => undef,
            OUT => undef,
            MAC => undef,
            SRC => undef,
            DST => undef,
            LEN => undef,
            TOS => undef,
            PREC => undef,
            TTL => undef,
            ID => undef,
            DF => undef,
            PROTO => undef,
            SPT => undef,
            DPT => undef,
            WINDOW => undef,
            RES => undef,
            CWR => undef,
            ECE => undef,
            SYN => undef,
            URGP => undef,
        );
        my $timestamp = $1;
        my $ufwmsg = $2;
        my $stuff = $3;
        print "Got time " . BOLD . "$timestamp" . RESET . ", ufw says " .
            BOLD . "$ufwmsg" . RESET . " and stuff is " . BOLD . "$stuff" .
            RESET . "\n" if ($debug gt 1);

        foreach my $k (keys %packet) {
            print "$k\n";
        }
        exit;
    }
}
