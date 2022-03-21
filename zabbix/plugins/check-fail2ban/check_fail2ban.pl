#!/usr/bin/perl
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker

use strict;
use warnings;
use Getopt::Long;

sub help {
    my $exitcode = 0;
    my $s = shift;
    if (defined($s)) {
        print "Error: " . $s . "\n";
        $exitcode=1;
    }
    print<<EOT;
Usage: check_fail2ban.pl [--help | --currently-failed | --total-failed | --currently-banned | --total-banned | --all | --help ] jailname

    --help              This help
    --verbose | -v      Don't be brief or distinct, but rather spend the day explaining what may be going wrong or right
                        and make sure you tell the user/admin every detail available because otherwise she or he might
                        be unsure of what to do and will be walking around in distress and probably become a drug addict
                        or worse before the day is over.
    --currently-failed  Show currently failed logins
    --total-failed      Show total failed logins
    --currently-banned  Show currently banned logins
    --total-banned      Show total banned logins
    --all               Show all of the above (default)

    The result is shown with number only, except for with --all, where there are labels per line.
    All arguemnts except --verbose are exclusive.

EOT
    exit($exitcode);
}

my ($currently_failed, $total_failed, $currently_banned, $total_banned);
my ($opt_verbose, $opt_cfailed, $opt_tfailed, $opt_cbanned, $opt_tbanned, $opt_all, $opt_help) = 0;
my $modes = 0;
my $cmd = "fail2ban-client status %s";

Getopt::Long::Configure('bundling');
GetOptions(
    "verbose+"              => \$opt_verbose, "v+" => \$opt_verbose,
    "--currently-failed"    => \$opt_cfailed, "f"  => \$opt_cfailed,
    "--total-failed"        => \$opt_tfailed, "F"  => \$opt_tfailed,
    "--currently-banned"    => \$opt_cbanned, "b"  => \$opt_cbanned,
    "--total-banned"        => \$opt_tbanned, "B"  => \$opt_tbanned,
    "--all"                 => \$opt_all,     "A"  => \$opt_all,
    "help"                  => \$opt_help,    "h"  => \$opt_help,
) or die "Invalid argument";

&help if (defined($opt_help));

# Sanity check
$modes++ if (defined($opt_cfailed));
$modes++ if (defined($opt_tfailed));
$modes++ if (defined($opt_cbanned));
$modes++ if (defined($opt_tbanned));

if ($modes == 0) {
    $opt_all = 1;
} elsif ($modes gt 1) {
    print STDERR "Please only state one mode\n";
    exit(1);
}

my $jailname = shift;
if (!defined($jailname) or $jailname eq "") {
    print STDERR "Invalid or missing jail name\n";
    exit(2);
}
my $runcmd = sprintf($cmd, $jailname);

open my $fd,"$runcmd|";
while (my $line = <$fd>) {
    if ($line =~ /Currently failed:\s+(\d+)/) {
        $currently_failed = $1;
    }
    elsif ($line =~ /Total failed:\s+(\d+)/) {
        $total_failed = $1;
    }
    elsif ($line =~ /Currently banned:\s+(\d+)/) {
        $currently_banned = $1;
    }
    elsif ($line =~ /Total banned:\s+(\d+)/) {
        $total_banned = $1;
    }
}
close($fd);

if (defined($opt_cfailed)) {
    print "$currently_failed\n";
} elsif (defined($opt_tfailed)) {
    print "$total_failed\n";
} elsif (defined($opt_cbanned)) {
    print "$currently_banned\n";
} elsif (defined($opt_tbanned)) {
    print "$total_banned\n";
} elsif (defined($opt_all)) {
    print "Currently failed: $currently_failed\n";
    print "Total failed $total_failed\n";
    print "Currently banned $currently_banned\n";
    print "Total banned $total_banned\n";
} else {
    print STDERR "wtf?\n";
    exit 4
}
exit 0
