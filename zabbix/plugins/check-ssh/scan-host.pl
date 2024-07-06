#!/usr/bin/perl
# Docs etc {{{
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker
#
# SSH encryption version check by Roy Sigurd Karlsbakk <roy@karlsbakk.net>
#
# See README.md for more docs and LICENSE.md for the license (which apparently
# is AGPLv3).
#
# }}}
# Sample output {{{
#
# PORT   STATE SERVICE
# 22/tcp open  ssh
# | ssh2-enum-algos:
# |   kex_algorithms: (11)
# |       sntrup761x25519-sha512@openssh.com
# |       curve25519-sha256
# |       curve25519-sha256@libssh.org
# |       ecdh-sha2-nistp256
# |       ecdh-sha2-nistp384
# |       ecdh-sha2-nistp521
# |       diffie-hellman-group-exchange-sha256
# |       diffie-hellman-group16-sha512
# |       diffie-hellman-group18-sha512
# |       diffie-hellman-group14-sha256
# |       kex-strict-s-v00@openssh.com
# |   server_host_key_algorithms: (4)
# |       rsa-sha2-512
# |       rsa-sha2-256
# |       ecdsa-sha2-nistp256
# |       ssh-ed25519
# |   encryption_algorithms: (6)
# |       chacha20-poly1305@openssh.com
# |       aes128-ctr
# |       aes192-ctr
# |       aes256-ctr
# |       aes128-gcm@openssh.com
# |       aes256-gcm@openssh.com
# |   mac_algorithms: (10)
# |       umac-64-etm@openssh.com
# |       umac-128-etm@openssh.com
# |       hmac-sha2-256-etm@openssh.com
# |       hmac-sha2-512-etm@openssh.com
# |       hmac-sha1-etm@openssh.com
# |       umac-64@openssh.com
# |       umac-128@openssh.com
# |       hmac-sha2-256
# |       hmac-sha2-512
# |       hmac-sha1
# |   compression_algorithms: (2)
# |       none
# |_      zlib@openssh.com
#
# }}}
# Libs etc {{{

use strict;
use warnings;

# }}}
# Globals {{{

# The good {{{
my %good_kex_algorithms = (
    'curve25519-sha256'                     => 1,
    'curve25519-sha256@libssh.org'          => 1,
    'diffie-hellman-group-exchange-sha256'  => 1,
    'diffie-hellman-group14-sha256'         => 1,
    'diffie-hellman-group15-sha512'         => 1,
    'diffie-hellman-group16-sha512'         => 1,
    'ecdh-sha2-nistp256'                    => 1,
    'ecdh-sha2-nistp384'                    => 1,
    'ecdh-sha2-nistp521'                    => 1,
);
my %good_server_host_key_algorithms = (
    'ecdsa-sha2-nistp256'                   => 1,
    'ecdsa-sha2-nistp384'                   => 1,
    'ecdsa-sha2-nistp521'                   => 1,
    'rsa-sha2-256'                          => 1,
    'rsa-sha2-512'                          => 1,
    'sk-ecdsa-sha2-nistp256@openssh.com'    => 1,
    'sk-ssh-ed25519@openssh.com'            => 1,
    'ssh-ed25519'                           => 1,
    'ssh-rsa-sha256@ssh.com'                => 1,
);
my %good_encryption_algorithms = (
    'aes128-gcm@openssh.com'                => 1,
    'aes256-gcm@openssh.com'                => 1,
    'chacha20-poly1305@openssh.com'         => 1,
    # When combined with hmac-sha2-512-etm@openssh.com, hmac-sha2-256-etm@openssh.com or umac-128-etm@openssh.com MACs, these are also ok
    # 'aes128-ctr'                          => 1,
    # 'aes192-ctr'                          => 1,
    # 'aes256-ctr'                          => 1,
);
my %good_mac_algorithms = (
    'none'                                  => 1,
    'zlib@openssh.com'                      => 1,
);
my %good_compression_algorithms = ();

# }}}
# The bad {{{

my %bad_kex_algorithms = (
    'diffie-hellman-group-exchange-sha1'    => 1,
    'diffie-hellman-group-exchange-sha256'  => 1,
    'diffie-hellman-group14-sha1'           => 1,
    'diffie-hellman-group17-sha512'         => 1,
    'diffie-hellman-group18-sha512'         => 1,
);
my %bad_server_host_key_algorithms = (
    'ssh-rsa'                               => 1,
    'ssh-dsa'                               => 1,
);
my %bad_encryption_algorithms = (
    '3des-cbc'                              => 1,
    '3des-ctr'                              => 1,
    'aes128-cbc'                            => 1,
    'aes128-cbc'                            => 1,
    'aes192-cbc'                            => 1,
    'aes256-cbc'                            => 1,
    'arcfour'                               => 1,
    'arcfour128'                            => 1,
    'arcfour256'                            => 1,
    'blowfish'                              => 1,
    'hmac-sha1'                             => 1,
    'hmac-sha2-256'                         => 1,
    'hmac-sha2-512'                         => 1,
    'twofish-cbc'                           => 1,
    'twofish128-cbc'                        => 1,
    'twofish128-ctr'                        => 1,
    'twofish256-cbc'                        => 1,
    'twofish256-ctr'                        => 1,
    'umac-128@openssh.com'                  => 1,
    'umac-64-etm@openssh.com'               => 1,
    'umac-64@openssh.com'                   => 1,
);
my %bad_mac_algorithms = (
    'hmac-md5'                              => 1,
    'hmac-md5-96'                           => 1,
    #'hmac-ripemd160'                        => 1,
    #'hmac-ripemd160@openssh.com'            => 1,
    'hmac-sha1'                             => 1,
    'hmac-sha1-96'                          => 1,
    #'umac-64@openssh.com'                   => 1,
);
my %bad_compression_algorithms = ();

# }}}
# And the ugly? {{{

my @unknown_kex_algorithms = undef;
my @unknown_server_host_key_algorithms = undef;
my @unknown_encryption_algorithms = undef;
my @unknown_mac_algorithms = undef;
my @unknown_compression_algorithms = undef;

# }}}
# Ymse {{{

my %protocol = (
    'v4' => '',
    'v6' => '-6',
);
my $cmd_skel = "nmap -p %d --script ssh2-enum-algos --script-args=\"ssh.user=%s\" %s";

# }}}

# }}}
# Main code {{{

# nmap -p ${port} --script ssh2-enum-algos --script-args="ssh.user=${username}" ${host}

exit 0;

# }}}
