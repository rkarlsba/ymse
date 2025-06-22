#!/opt/homebrew/Cellar/perl/5.40.1/bin/perl
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker

use strict;
use warnings;
use LWP;
#use LWP::UserAgent;

print "This is libwww-perl-$LWP::VERSION\n";

exit;

my $ua = LWP::UserAgent->new;
$ua->agent("ZabbixCheck/0.1 ");

# Create a request
my $req = HTTP::Request->new(GET => 'https://search.cpan.org/search');

# $req->content_type('application/x-www-form-urlencoded');
# $req->content('query=libwww-perl&mode=dist');

my $res = $ua->request($req);

if ($res->is_success) {
    print $res->content;
}
else {
    print $res->status_line, "\n";
}
