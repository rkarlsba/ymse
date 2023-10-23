#!/usr/bin/perl

my $debug = 0;
my $match='^\* ALPN: server accepted (to use )?h2$';

my $url = shift;

open (my $fd,"curl -vso /dev/null $url 2>&1|") ||
	die ("Can't open curl: $url $!\n");

my $support = 0;
my $line = 0;
while (my $s = <$fd>) {
	chomp($s);
	$line++;
	printf("[%02i] $s\n", $line) if ($debug gt 0);
	if ($s =~ /$match/) {
		$support++;
	}
}
if ($support gt 0) {
	printf("PROBLEM: Server supports HTTP/2%s\n", $debug ? " ($support)" : "");
} else {
	printf("OK: Server does not support HTTP/2\n");
}

