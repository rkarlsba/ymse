#!/usr/bin/perl
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker

my $fn = 'pure-bash-bible.md';
open(my $fh, '<', $fn)
    or die "Can't open < $fn $!";

# Process every line in input.txt
while (my $line = readline($fh)) {
    chomp($line);
    print($line);
}

