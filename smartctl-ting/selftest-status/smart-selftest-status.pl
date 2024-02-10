#!/usr/bin/perl
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker

use strict;
use warnings;
use Getopt::Long;

# Globals {{{

my $program_name = $0;
my $opt_filename = undef;
my $opt_help = 0;
my $opt_verbose = 0;
my $verbose = undef;
my $filename = undef;

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
    print("Usage: $program_name [ -f output_from_smartctl-a.txt ] [ -h ] [ -v [ -v [ ... ]]]\n");
    exit($exitcode);
}

# }}}
# Getopt {{{

Getopt::Long::Configure('bundling');
GetOptions(
    "v+"    => \$opt_verbose,   "verbose+"      => \$opt_verbose,
    "h"     => \$opt_help,      "help"          => \$opt_help,
    "f=s"   => \$opt_filename,  "filename=s"    => \$opt_filename,
) or &help("Invalid argument");

&help if ($opt_help);
$verbose=$opt_verbose if (defined($opt_verbose));
$filename=$opt_filename if (defined($opt_filename));

# }}}

&help('Stomach ache', 42);
