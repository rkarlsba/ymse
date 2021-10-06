#!/usr/bin/env perl
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker
#
#{{{
# {
#     "jsonrpc": "2.0",
#     "method": "user.login",
#     "params": {
#         "user": "Admin",
#         "password": "zabbix"
#     },
#     "id": 1,
#     "auth": null
# }
# }}}

use strict;
use warnings;
use JSON::XS;
use LWP::UserAgent;
use HTTP::Cookies;
use Data::Dumper;
use Mozilla::CA;

# Auth {{{
# I set these in authstuff.pl so noone on github can get to them, but they're
# grenerally set like this.
#
# my $api_user = "someuser";
# my $api_password = "somepass";
# my $api_url = "https://zabbie.my.tld/zabbix/api_jsonrpc.php";
#
# If you want to do like I do, just create functions like this one to return
# the data you want:
#
# sub get_api_url {
#     return "https://zabbix.my.tld/zabbix/api_jsonrpc.php";
# }
#
# }}}

require "./authstuff.pl";

my $api_user = &get_api_user;
my $api_password = &get_api_password;
my $api_url = &get_api_url;
my $api_id = 1;
my $api_auth = undef;
#my $content_type = 'application/json';
my $content_type = "application/json-rpc";

my $api_auth_text = defined($api_auth) ? "\"$api_auth\"" : "null";

my $login = {
    'jsonrpc' => '2.0',
    'method' => 'user.login',
    'params' => {
        'user' => $api_user,
        'password' => $api_password,
    },
    'id' => $api_id,
    'auth' => $api_auth_text,
};

my $http_req = HTTP::Request->new('POST', $api_url);
$http_req->header('Content-Type' => $content_type);
$http_req->content(encode_json($login));
my $lwp = LWP::UserAgent->new;
my $res = $lwp->request($http_req);

print Dumper($res);

#my $json = encode_json($login);

#print "$json\n";
