#!/usr/bin/perl
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker

# {{{ Docs
#
# filelist.txt is a list of my music files created with
# find /musicpath -type f -exec file {}\; > filetypes.txt
#
# }}}
# {{{ Example filetype list
#
# /data/empetre/.AppleDouble/.htaccess: AppleDouble encoded Macintosh file
# /data/empetre/.AppleDouble/.iTunes Preferences.plist: AppleDouble encoded Macintosh file
# /data/empetre/.AppleDouble/Hedningarna.zip: AppleDouble encoded Macintosh file
# /data/empetre/.AppleDouble/delillos.tar: AppleDouble encoded Macintosh file
# /data/empetre/.AppleDouble/finddup.db: AppleDouble encoded Macintosh file
# /data/empetre/16 Horsepower/.AppleDouble/.DS_Store: AppleDouble encoded Macintosh file
# /data/empetre/16 Horsepower/.AppleDouble/.Parent: AppleDouble encoded Macintosh file
# /data/empetre/16 Horsepower/16 Horsepower/.AppleDouble/.Parent: AppleDouble encoded Macintosh file
# /data/empetre/16 Horsepower/16 Horsepower/01 Haw.mp3: Audio file with ID3 version 2.3.0, contains: MPEG ADTS, layer III, v1, 192 kbps, 44.1 kHz, Stereo
# /data/empetre/16 Horsepower/16 Horsepower/02 South Pennsylvania Waltz.mp3: Audio file with ID3 version 2.3.0, contains: MPEG ADTS, layer III, v1, 192 kbps, 44.1 kHz, Stereo
#
# }}}
# {{{ Example extensions and count
#
# .mp3: 29303
# .flac: 4770
# .m4a: 4509
# .wma: 171
# .m4p: 165
# .wav: 158
# .aif: 137
# .ogg: 92
# .m4b: 43
# .mp4: 17
# .mts: 14
# .aiff: 11
# .webm: 9
# .mov: 9
# .flv: 6
#
# }}}

use strict;
use warnings;
use File::Basename;

my %extensions;
my %descriptions;
my @files;
my @good_ext = ("mp3", "flac", "m4a", "wma", "m4p", "wav", "aif", "ogg", "m4b", "mp4", "mts", "aiff", "webm", "mov", "flv");
my $filetypes_fn = "filetypes.txt";
my $debug = 1;

sub is_zeroed {
    my $lfn = shift;
    local $/;
    my $br;

    open(my $lfh, "<", $lfn) ||
        die "Can't open file \"$lfn\": $!";
    my $filecontents = <$lfh>;
    close($lfh);

    return ($filecontents =~ m/^\x00+$/);
}

open(my $fh, "<", $filetypes_fn) ||
    die "Can't open file \"$filetypes_fn\": $!";

while (my $s = <$fh>) {
    my ($fn,$bn,$dir,$ext,$desc);

    chomp($s);
    if ($s =~ m/^(.*?):\s(.*)/) {
        $fn = $1;
        $desc = $2;

        ($bn, $dir, $ext) = fileparse($fn, qr/\.[^.]*/);
        $ext = lc($ext);
        $ext =~ s/^\.//;
        next unless (grep( /^$ext$/, @good_ext));
        next unless ($desc eq "data");
        print("Found \"$bn\" and ext \"$ext\"\n") if ($debug gt 1);
        push @files,$fn;
        print("$fn\n") if ($debug gt 1);
        $descriptions{$desc}++;
        $extensions{$ext}++;
    }
}

close($fh);

# Debug shite {{{
if ($debug gt 1) {
    foreach my $ext (sort { $extensions{$b} <=> $extensions{$a} } keys %extensions) {
        print "$ext: $extensions{$ext}\n"
    }
}

if ($debug gt 1) {
    foreach my $desc (sort { $descriptions{$b} <=> $descriptions{$a} } keys %descriptions) {
        print "$desc $descriptions{$desc}\n"
    }
}
# }}}

foreach my $fn (@files) {
    my $z = is_zeroed($fn) ? "Z" : "-";
    print "[$z] $fn\n";
}
