#!/usr/bin/perl
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker

use strict;
use warnings;
use Getopt::Long;

# Globals
my $opt_attr = undef;
my $opt_force = undef;
my $opt_help = undef;
my $opt_verbose = undef;

my $attr = undef;
my $errcnt = 0;
my $program_name = $0;

# Subroutines
sub help {
    my $exitcode = 0;
    my $s = shift;
    if (defined($s)) {
        chomp($s);
        printf("$s\n");
        $exitcode++;
    }
    printf("Usage: $program_name -a someattr /dev/sdX\n\n");
    printf("Having someattr being either a numeric ID (hex, oct or decimal). Please\n");
    printf("run smartctl -A /dev/sdX to see all available for that particular disk.\n");
    printf("Different disk typs (HDDs, SSDs etc) and vendors usually have different\n");
    printf("attributes, although there's a set of common ones usually available regardless\n");
    printf("of model.\n");
    exit($exitcode);
}

sub num2dec {
    my $lattr = shift;
    return undef unless (defined($opt_attr));
    return $lattr if ($lattr =~ /^\d+$/0); # it's already decimal
    if ($lattr =~ /^0x([0-9a-f]+)$/i) {
        return hex($lattr);
    } elsif ($lattr =~ /^0(\d+)$/) {
        return oct($lattr);
    }
    return undef;
}

# Parse args
Getopt::Long::Configure('bundling');
GetOptions(
    "attr=s"    => \$opt_attr,    "a=s" => \$opt_attr,
    "force"     => \$opt_force,   "f"   => \$opt_force,
    "help"      => \$opt_help,    "h"   => \$opt_help,
    "verbose+"  => \$opt_verbose, "v+"  => \$opt_verbose,
) or help("Invalid argument");

&help if (defined($opt_help));
&help("Error: We need an attribute") unless (defined($opt_attr));

$attr = &num2dec($opt_attr);
&help("Error: Invalid attribute - must be a number\n") unless (defined($attr));

# 197 Current_Pending_Sector  0x0012   100   100   000    Old_age   Always       -       0
while (my $dev = shift) {
    if ( -b $dev ) {
        my $cmd = "smartctl -A $dev";
        print "Running command \"$cmd\"\n";
        open my $smrt,"$cmd|";
        if (defined($smrt) && ($smrt)) {
            while (my $smrtline = <$smrt>) {
                chomp($smrtline);
                my @smrtarr = split(/\s+/, $smartline);
                if ($smartarr[0] eq $attr) {
                    my $attrname = $smartattr[1];
                    $attrname =~ s/_//g;
                    print
            }
        }
    } else {
        print STDERR "$dev is not a block device\n";
        $errcnt++;
    }
}

if ($errcnt) {
    print STDERR "$errcnt errors found. Please see above for details.\n";
    exit $1;
}
exit 0;
