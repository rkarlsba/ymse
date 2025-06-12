#!/usr/bin/perl
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker

my $fn = 'pure-bash-bible.md';
open(my $fh, '<', $fn)
    or die "Can't open < $fn $!";

my $found=0;
while (my $line = readline($fh)) {
    chomp($line);
    if ($found) {
        if ($line =~ /^```$/) {
            $found=0;
            continue;
        } else {
            print("$line\n");
        }
    } elsif ($line =~ /^```sh$/) {
        $found=1;
        print("$line\n");
    }
}

