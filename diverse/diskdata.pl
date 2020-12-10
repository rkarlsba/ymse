#!/usr/bin/perl

use strict;
use warnings;

my $md = `cat /proc/mdstat`;

print($md);
