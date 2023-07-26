#!/usr/bin/perl
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker

use strict;
use warnings;

# se ps-axfv.txt for ps-output. Eksempel under
#
#   PID TTY      STAT   TIME  MAJFL   TRS   DRS   RSS %MEM COMMAND
#     2 ?        S      0:00      0     0     0     0  0.0 [kthreadd]
#     3 ?        I<     0:00      0     0     0     0  0.0  \_ [rcu_gp]
#     4 ?        I<     0:00      0     0     0     0  0.0  \_ [rcu_par_gp]

my %pstree;
my $debug = 0;
my $cmd = 'ps axfv';

if ($^O != "linux") {
    $debug = 1;
    print(STDERR "Not sure about how to parse 'ps' output on $^O - switching to debug mode.\n");
}

if ($debug gt 0) {
    $cmd = 'ps-axfv.txt';
} else {
    $cmd = 'ps axfv';
}

open(my $cmd_fd, "$cmd|") ||
    die("Can't run $cmd: $!\n");

while (my $line = <$cmd_fd>) {
    chomp($line);
    print($line . "\n");
#                     PID       TTY          STAT           TIME      MAJFL      TRS     DRS     RSS      %MEM      COMMAND
    if ($line =~ /\s*(\d+)\s+([\w\?\/]+)\s+([\w\<\+]+)\s+(\d+\:\d{2})\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+\.\d+)\s(.*)/) {
#            1    2    3     4     5    
        my ($pid,$tty,$stat,$time,$majfl,$trs,$drs,$rss,$pstmem,$command) = ($1,$2,$3,$4,$5,$6,$7,$8,$9,$10,
        $pstree[
    }
}

close($cmd_fd);
