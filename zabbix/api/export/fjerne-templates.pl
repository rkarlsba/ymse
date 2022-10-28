#!/usr/bin/bash

use strict;
use warnings;

my $infn="zbx_export_hosts.xml";
my $outfn="zbx_export_hosts-vaska.xml";

open(my $inf,$infn) || die "$!";
open(my $outf,">$outfn") || die "$!";

close($inf);
close($outf);
