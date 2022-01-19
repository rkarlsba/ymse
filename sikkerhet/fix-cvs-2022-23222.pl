#!/usr/bin/perl
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker

use strict;
use warnings;

# Stuff
my $verbose = 1;

# Fetch current value
my $keyname = "kernel.unprivileged_bpf_disabled";
my $unprivileged_bpf_disabled = `sysctl -n $keyname`;
chomp($unprivileged_bpf_disabled);
print "unprivileged_bpf_disabled is $unprivileged_bpf_disabled.\n" if ($verbose gt 0);

if ($unprivileged_bpf_disabled == 0) {
	print "ERROR: Security hole wide open\n" if ($verbose gt 0);
} elsif ($unprivileged_bpf_disabled == 1) {
	print "OK: Security hole firmly closed and can't be opened without a reboot\n" if ($verbose gt 0);
} elsif ($unprivileged_bpf_disabled == 2) {
	print "OK: Security hole closed, but can be open by setting $keyname to 0\n" if ($verbose gt 0);
}
