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

# brukernavn og passord tar vi fra kommandolinja etter hvert, men
# vi begynner med hardkoding

# my $api_user = "secretuser";
# my $api_password = "secretpassword";
# I set these in authstuff.pl so noone on github can get to them :þ

require "authstuff.pl";

my $api_id = 1;
my $api_auth = undef;

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
