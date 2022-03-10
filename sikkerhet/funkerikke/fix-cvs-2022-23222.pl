#!/usr/bin/perl
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker

use strict;
use warnings;
use Getopt::Long;

# Stuff
my $o_verbose = 0;
my $o_quiet = 0;
my $o_do_fix = 0;
my $o_just_report = 0;
my $o_value = 2;
my $o_help = 0;
my $do_fix = 0;
my $verbose = 0;
my $errors = 0;
my $errmsg = undef;

my $kernel_filename = '/proc/sys/kernel/unprivileged_bpf_disabled';

END {
   close KFH;
}

sub help {
    print "fix-CVE-2022-23222.pl - a temporary fix for CVE-2022-23222 - the bug with all the two's\n";
    print "Syntax: fix-CVE-2022-23222.pl [--verbose] [--ddo-fix] [--value n]\n";
    print "  --verbose -V   Talk freely about what you do\n";
    print "  --quiet -q     Opposite of --verbose\n";
    print "  --just-report -r Do not fix, just report. Implies --verbose\n";
    print "  --do_fix -f    Check, but not fix. Does not imply (but does not block) --verbose\n";
    print "  --value -v n   <n> is the value to 'fix' the error. 1 for hard fix and 2 for soft fix.\n";
    print "  --help -h      This help.\n";
    print "\n";
    print "  A hard fix will need a reboot to recover from it. A soft fix can be set through\n";
    print "  procfs. Only newer kernels support the latter. By default, this script will attept to\n";
    print "  set this value to 2 and if that fails, revert to 1. If it's set to 1 already, the script\n";
    print "  will ignore it.\n";
    print "\n";
    print "Notes:\n";
    print "  --value is not implemnteted.\n";
    print "  --verbose is implicit without do_fix.\n";
    exit(0);
}

sub disable_fix_unprivileged_bpf_disabled {
    open(KFH, '>',$kernel_filename) || die ("Can't open $kernel_filename for writing $!\n");
    # Try 2 first
    if (print KFH "2\n") {
        close(KFH);
        print "fix_unprivileged_bpf_disabled set to 2\n" if ($verbose);
        return 2;
    } else {
        print "Could not set fix_unprivileged_bpf_disabled to 2 - old kernel, perhaps?" if ($verbose);
        if (print KFH "1\n") {
            close(KFH);
            print "fix_unprivileged_bpf_disabled set to 2. A reboot is needed to reset this." if ($verbose);
            return 1;
        } else {
            print STDERR "Could not set fix_unprivileged_bpf_disabled to neither 2 nor 1. Giving up\n";
            exit 2;
        }
    } 
    close KFH;
}

Getopt::Long::Configure ("bundling");
GetOptions(
    'h|help'        => \$o_help,
    'V|verbose'     => \$o_verbose,
    'q|quiet'       => \$o_quiet,
    'f|do-fix'      => \$o_do_fix,
    'v|value=i'     => \$o_value,
    'r|just-report' => \$o_just_report
);

&help if ($o_help gt 0);

# Sanity check
if ($o_verbose gt 0 and $o_quiet gt 0 ) {
    print STDERR "Both quiet and verbose? Are you quite sane?\n";
    $errors++;
}

if ($o_do_fix gt 0 and $o_just_report gt 0) {
    print STDERR "Will report silently to /dev/null while failing not to reporting because of do_fix and being rather quietly nervously verbose about it?\n";
    $errors++;
}

$do_fix = $o_do_fix;
$verbose = $o_verbose;

if ($o_just_report gt 0) {
    $do_fix = 0;
    $verbose = 1;
} elsif ($o_do_fix gt 0) {
    $do_fix = 1;
}

# Default to verbose if not eplicitly quiet
if ($verbose eq 0 and $o_do_fix eq 0 and $o_just_report eq 0) {
    $verbose = 1;
}

# $just_report = 1 if (!$do_fix and !$just_report);

if ($o_do_fix and $> ne 0) {
    print STDERR "We need to be r00t to run this script to change anything\n";
    $errors++;
}

if ($errors gt 0) {
    print STDERR "PEBKAC: Some errors were observed in the commandline!\n";
    exit(1);
}


# Fetch current value
open(my $kernfile,"<", $kernel_filename) or die "Can't open kernel file $kernel_filename $!\n";
my $unprivileged_bpf_disabled = <$kernfile>;
close $kernfile;

chomp($unprivileged_bpf_disabled);
$errmsg .= "unprivileged_bpf_disabled is $unprivileged_bpf_disabled.\n";

# Report current value
if ($unprivileged_bpf_disabled == 0) {
	print "ERROR: Security hole wide open";
    print $errmsg if (defined($errmsg));
    print "\n";
} elsif ($unprivileged_bpf_disabled == 1) {
	print "OK: Security hole firmly closed and can't be opened without a reboot\n" if ($verbose gt 0);
} elsif ($unprivileged_bpf_disabled == 2) {
	print "OK: Security hole closed, but can be open by setting keyname to 0\n" if ($verbose gt 0);
}

# Fix current value
if ($do_fix gt 0) {
    if ($unprivileged_bpf_disabled == 0) {
        my $val = disable_fix_unprivileged_bpf_disabled();
        print "OK: Security hole firmly disabled with a value $val\n" if ($verbose gt 0);
    } elsif ($unprivileged_bpf_disabled == 1) {
        print "OK: Security hole firmly closed and can't be opened without a reboot\n" if ($verbose gt 0);
    } elsif ($unprivileged_bpf_disabled == 2) {
        print "OK: Security hole closed, but can be open by setting keyname to 0\n" if ($verbose gt 0);
    }
}
0
