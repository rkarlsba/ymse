#!/usr/bin/perl
# vim:ts=4:sw=4:et:ai:cindent:fdm=marker
#
# Written by Roy Sigurd Karlsbakk <roy@karlsbakk.net>
#
# Lincensed under GPL v2. See
# http://www.gnu.org/licenses/old-licenses/gpl-2.0.en.html for details.
#
#

use strict;
use warnings;
use Date::Manip;
use POSIX qw(strftime);

# Connecting to Director urd.hioa.no:9101 {{{
# 1000 OK: urd.hioa.no-dir Version: 14.2.2 (12 December 2014)
# Enter a period to cancel a command.
# list volumes pool=Full
# Automatically selected Catalog: MyCatalog
# Using Catalog "MyCatalog"
# +---------+------------+-----------+---------+-------------------+----------+--------------+---------+------+-----------+-----------+---------------------+
# | mediaid | volumename | volstatus | enabled | volbytes          | volfiles | volretention | recycle | slot | inchanger | mediatype | lastwritten         |
# +---------+------------+-----------+---------+-------------------+----------+--------------+---------+------+-----------+-----------+---------------------+
# |       1 | Full-0001  | Append    |       1 |     1,045,128,471 |        0 |   31,536,000 |       1 |    0 |         0 | File      | 2015-05-29 11:47:31 |
# |       3 | IO6741L6   | Full      |       1 | 2,489,395,525,632 |      258 |   31,536,000 |       1 |    0 |         0 | LTO6      | 2015-06-04 07:38:45 |
# |      97 | IO6750L6   | Full      |       1 | 1,857,171,879,936 |      201 |   31,536,000 |       1 |    0 |         0 | LTO6      | 2015-05-31 23:53:51 |
# |     105 | IO6281L6   | Full      |       1 | 2,363,925,438,464 |      245 |   31,536,000 |       1 |    0 |         0 | LTO6      | 2015-06-02 18:56:20 |
# |     121 | IO6282L6   | Full      |       1 | 2,497,998,489,600 |      254 |   31,536,000 |       1 |    0 |         0 | LTO6      | 2015-06-04 01:34:39 |
# }}}

my $noignore = 1;
my $day = (60*60*24);
my $month = ($day * 30);
my $pool = "UNKNOWN";
my %pools_ignored = (
    'Incremental' => 1,
);
my %volstatuses_ignored = (
    'Cleaning' => 1,
);
my $cmdline = "echo 'list volumes' | bconsole";

open BCONSOLE,"$cmdline|" || die "Could not run command '$cmdline': $!\n";

while (<BCONSOLE>) {
    my $lastwritten_s;
    my $timestamp = undef;
    my $expiry_ts = 0;
    my $expiry_s = '';
    my $s = ' ';

    if (/^Pool: (\w+)/) {
        $pool = $1;
    }

    next if ($pools_ignored{$pool} and not $noignore);
    next unless (/^\|/);
    s/\|//g;
    s/^\s+//;
    s/\s+$//;
    s/,//g;
    my ($mediaid, $volumename, $volstatus, $enabled, $volbytes, $volfiles, $volretention, $recycle, $slot, $inchanger, $mediatype, $lr_date, $lr_time) = split(/\s+/);
    next if ($mediaid eq 'mediaid');
    next if ($volstatuses_ignored{$volstatus} and not $noignore);
    if (defined ($lr_date) && defined ($lr_time)) {
        $lastwritten_s = "last written to $lr_date $lr_time";
        $timestamp = UnixDate(ParseDate("$lr_date $lr_time"), "%s");
        $expiry_ts = $volretention+$timestamp;
        if ($expiry_ts < time) {
            $lastwritten_s .= " EXPIRED";
            $expiry_s = "EXPIRED ";
        } else {
            $expiry_s = "Expires ";
        }
        $expiry_s .= strftime('%F', localtime($expiry_ts));
    } else {
        $lastwritten_s = "Unused";
    }
    my $volretmon = sprintf("%d", $volretention/$month);
    my $volretday = sprintf("%d", $volretention/$day);
    #$s = 's' if ($volretmon != 1);
    $s = 's' if ($volretday != 1);
    printf("Pool: %-13s Media: %-6s Filename: %-23s Slot: %-2d Retention: %2d Day%s %s\n", $pool, $mediaid, $volumename, $slot, $volretday, $s, $expiry_s);
#printf("Pool %-12s Media %-6s Retention %-2d Month%s %s\n", $pool, $mediaid, $volretmon, $s, $lastwritten_s);
}

close BCONSOLE;
