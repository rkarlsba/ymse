#!/usr/bin/perl
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker

my $fn = 'pure-bash-bible.md';
open(my $fh, '<', $fn)
    or die "Can't open < $fn $!";

my $found=0;
while (my $line = readline($fh)) {
    chomp($line);
    next if ($line =~ /^\s*#.*/);
    if ($found) {
        if ($line =~ /^```$/) {
            $found=0;
            print("\n");
            next;
        } else {
            print("$line\n");
        }
    } elsif ($line =~ /^```sh$/) {
        $found=1;
    }
}

