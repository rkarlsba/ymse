#!/usr/bin/perl
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker
#
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
