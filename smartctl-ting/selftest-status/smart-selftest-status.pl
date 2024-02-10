#!/usr/bin/perl
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker

use strict;
use warnings;
use Getopt::Long;

# Docs and comments {{{
# 
# Self-test execution status:      (   0)   The previous self-test routine completed
#                                           without error or no self-test has ever 
#                                           been run.
# 
# Self-test execution status:      ( 241)   Self-test routine in progress...
#                                           10% of test remaining.
# 
# }}}
# Globals {{{

my $program_name = $0;
my $opt_filenames = undef;
my $opt_help = 0;
my $opt_verbose = 0;
my $verbose = undef;
my $filename = undef;

# }}}
# sub devinfo($devname) {{{

sub devinfo {
    my $fd;
    my $args = @_;
    return $args if ($args lt 1);
    my $devname = $_[0];
    my $foundstatus = 0;
    my $statuscode = undef;
    my $statustext = undef;

    if ( -f $devname ) {
        open $fd,$devname || 
            die "Error opening file \"$devname\": $!";
    } elsif ( -b $devname ) {
        open $fd,"smartctl -x $devname|" || 
            die "Error opening file \"$devname\": $!";
    }
    while (my $s = <$fd>) {
        chomp($s);
        if ($foundstatus) {
            if ($s =~ /^\s\s\s+(\w.*)/) {
                $statustext .= " $1";
            } else {
                last;
            }
        } elsif ($s =~ /^Self-test execution status:\s+\(\s*(\d+)\s*\)\s+(.*)/) {
            $statuscode = $1;
            $statustext = $2;
            $foundstatus++;
        }
    }
    $statustext =~ s/\s\s+/ /g;
    print "$devname: [$statuscode]: $statustext\n";
}

# }}}
# sub help([errstr [, exitcode]]) {{{

sub help {
    my $args = @_;
    my $s = undef;
    my $exitcode = 1;

    $s .= $_[0] if ($args gt 0);
    $exitcode = $_[1] if ($args gt 1);
    $s .= " and help() was called with $args arguments, which is confusing" if ($args gt 2);

    print "ERROR: $s\n" if (defined($s));
    print("Usage: $program_name [ -h ] [ -v [ -v [ ... ]]] /dev/sdX [ /dev/sdY [ /dev/sdZ [ ... ] ] ] [ /file/to/smartctl-x/output.txt [ /another/output/file.txt [ ... ] ] \n");
    exit($exitcode);
}

# }}}
# Getopt {{{

Getopt::Long::Configure('bundling');
GetOptions(
    "v+"    => \$opt_verbose,   "verbose+"      => \$opt_verbose,
    "h"     => \$opt_help,      "help"          => \$opt_help,
) or &help("Invalid argument");

&help if ($opt_help);
$verbose=$opt_verbose if (defined($opt_verbose));

# }}}

my $args_left = $#ARGV+1;
&help("Got $args_left args - I need a dev or a smartctl -a output file!") if ($args_left le 0);

while (my $fn = shift) {
    &devinfo($fn);
}
