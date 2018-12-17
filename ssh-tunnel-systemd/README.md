ssh-tunnel-systemd
==================

## Open SSH tunnel and keep it open using systemd

Copy thie files here into their given locations. The file
**/etc/systemd/system/secure-tunnel@.service** is the service template; the file
**/etc/default/secure-tunnel@test.my.tld** is an example for the host **test.my.tld**.
The tunnel is openened by user root, logging into the remote system with the given
remote user, defaulting here to ${HOSTNAME}\_tun. The example code will initiate a
tunnel from the local machine to the remote, allowing a user to login to the remote
machine's localhost:${LOCAL_PORT}, ending up at this machine on port 22, ssh.

