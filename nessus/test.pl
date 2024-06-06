#!/usr/bin/perl
# vim:ts=4:sw=4:sts=4:et:ai:si:fdm=marker

use strict;
use warnings;

my $fn = 'test';
my $bash = '/usr/bin/bash';

open my $fd,'>',$fn ||
    die "Can't open file $fn for writing: $!";

system($bash);

close($fd);
0

