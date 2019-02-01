#!/usr/bin/perl
# vim:ts=4:sw=4:sts=4:et:ai:cindent

use strict;
use warnings;

# 2019/01/31 00:26:44 ERROR : owncloud/data/malinux/files/Kindle-Backup/documents/Basic Statistics Part 3_ The Dangers of Large Data Sets_ A Tale of P values, Error Rates, and Bonferroni Corrections_IEJZLYRH6KVXXYQ4IUKBJBB6IFSS433D.azw: Failed to copy: error 400: no.jotta.backup.errors.InvalidArgumentException: JFS file names may not be longer than 255 characters (Bad Request)
# 2019/01/31 00:28:24 ERROR : owncloud/data/malinux/files/Kindle-Backup/documents/Er du blitt kjent med en asperger, eller plutselig funnet ut at en av (de litt mer saere) vennene du har kjent en stund, er det - Marlen Hegna Foss.pdf-cdeKey_H4PNA7GZSTEDP44WMLST3JZHZPZCJXSB.pdf: Failed to copy: error 400: no.jotta.backup.errors.InvalidArgumentException: JFS file names may not be longer than 255 characters (Bad Request)

while (my $line = <STDIN>) {
    if ($line =~ /^.* ERROR : (.*?): Failed to copy.*JFS/) {
        my $path = $1;
        my @dtmp = split('/', $path);
        my $filename = $dtmp[$#dtmp];
        print "$path";
        print "$filename\n\n";
    }
}
