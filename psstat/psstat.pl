#!/usr/bin/perl
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker

use strict;
use warnings;
  use Getopt::Long;

my %pstree;
my $debug = 0;
my $cmd = 'ps axfv';
my $run = 'run';

if ($^O ne "linux") {
    $debug = 1;
    $run = 'open';
    print(STDERR "Not sure about how to parse 'ps' output on $^O - switching to debug mode.\n");
}

if ($debug gt 0) {
    $cmd = 'ps-axfv.txt';
} else {
    $cmd = 'ps axfv';
}

open(my $cmd_fd, "$cmd") ||
    die("Can't $run '$cmd': $!\n");

while (my $line = <$cmd_fd>) {
#   chomp($line);
#   print($line . "\n");
#                     PID       TTY          STAT           TIME      MAJFL      TRS     DRS     RSS      %MEM      COMMAND
    if ($line =~ /\s*(\d+)\s+([\w\?\/]+)\s+([\w\<\+]+)\s+(\d+\:\d{2})\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+\.\d+)\s(.*)/) {
#            1    2    3     4     5    
        my ($pid,$tty,$stat,$time,$majfl,$trs,$drs,$rss,$pstmem,$command) = ($1,$2,$3,$4,$5,$6,$7,$8,$9,$10);
        $pstree{$pid}{'tty'} = $tty;
        $pstree{$pid}{'stat'} = $stat;
        $pstree{$pid}{'time'} = $time;
        $pstree{$pid}{'majfl'} = $majfl;
        $pstree{$pid}{'trs'} = $trs;
        $pstree{$pid}{'drs'} = $drs;
        $pstree{$pid}{'rss'} = $rss;
        $pstree{$pid}{'pstmem'} = $pstmem;
        $pstree{$pid}{'command'} = $command;
    }
}

close($cmd_fd);
