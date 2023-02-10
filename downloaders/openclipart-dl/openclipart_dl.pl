#!/usr/bin/env perl
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker:cindent

use strict;
use warnings;
# use LWP::Curl;
use LWP::Simple;

# example 'openclipart_dl.pl https://openclipart.org/detail/65569/johnny-coffee-2'
#
# This will download the file as follows
# https://openclipart.org/detail/65569/johnny-coffee-2'
# https://openclipart.org/download/65569        -> johnny-coffee-2.svg
# https://openclipart.org/image/400px/65569     -> johnny-coffee-2-400px.png
# https://openclipart.org/image/800px/65569     -> johnny-coffee-2-800px.png
# https://openclipart.org/image/2000px/65569    -> johnny-coffee-2-2000px.png

my ($svg, $png4, $png8, $png20, $url);
my %downloads;

while ($url = shift) {
    $svg = $png4 = $png8 = $png20 = $url;
    my ($prefix,$id,$filename);
    if ($url =~ /(http.*?)\/detail\/(\d+)\/(.*)/) {
        $prefix = $1;
        $id = $2;
        $filename = $3;
        system "wget -qO $filename.svg $prefix/download/$id\n";
        system "wget -qO $filename-400px.png $prefix/image/400px/$id\n";
        system "wget -qO $filename-800px.png $prefix/image/800px/$id\n";
        system "wget -qO $filename-2000px.png $prefix/image/2000px/$id\n";
    }
}

