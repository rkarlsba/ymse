<!--
vim:ts=4:sw=4:sts=4:et:ai:si:fdm=marker:tw=100
-->
ssh-tunnel-systemd
==================

## Open SSH tunnel and keep it open using systemd

Copy these files into their given locations, which are:

| Source file                               | Destination file                              |
| ----------------------------------------- | --------------------------------------------- |
| bin/ssh-tunnel.sh                         | /usr/local/bin/ssh-tunnel.sh                  |
| etc/systemd/system/secure-tunnel@.service | /etc/systemd/system/secure-tunnel@.service    |
| etc/default/secure-tunnel@test.my.tld     | /etc/default/secure-tunnel@test.my.tld        |

The file **/etc/systemd/system/secure-tunnel@.service** is the service template and the file
**/etc/default/secure-tunnel@test.my.tld** is an example for the host **test.my.tld**.  The tunnel
is openened by user root, logging into the remote system with the given remote user, defaulting here
to ${HOSTNAME}\_tun.  The example code will initiate a tunnel from the local machine to the remote,
allowing a user to login to the remote machine's localhost:${LOCAL_PORT}, ending up at this machine
on port 22, ssh.

Written by [Roy Sigurd Karlsbakk](mailto:roy@karlsbakk.net) around 2017 and updated every leap year
or so, latest in september 2025.

See LICENSE.md for details.

