#!/usr/bin/perl

use strict;
use warnings;
use URI::Fast qw(uri encode decode);

my $url = 'https%3A%2F%2Fgithub.com%2Fearlephilhower%2Farduino-pico%2Freleases%2Fdownload%2Fglobal%2Fpackage_rp2040_index.json&v=IZKpCz6LEdg';
my $uri = uri($url);

print "URL was \"$uri\"\n";
print "URL is  \"" . decode($uri) . "\"\n";
