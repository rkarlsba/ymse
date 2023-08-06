root@smilla:~# ssh -p 8822 localhost
#
# ========================================== DOCUMENTATION ======================================= {{{
# Flags with which to use with ssh
#  -N   Do not execute a remote command. This is useful for just forwarding ports.
#  -n   Redirects stdin from /dev/null (actually, prevents reading from stdin).
#  -R   Specifies that connections to the given TCP port on the remote (server)
#       host are to be forwarded to the local side.
# ssh opts
#   ServerAliveInterval 20
#       If ssh has nothing else to do, send a no-op null packet after 20 seconds
#       to keep the connection alive
#   ServerAliveCountMax 5
#       If the above no-op packet is sent 5 times in a row and not replied to by
#       the server, make a wild guess it's a broken pipe somewhere and disconnect
#       from the server to have systemd restart the ssh tunnel.

# This thing is to be used in systemd with something like this:

# [Unit]
# Description=SSH tunnel service
# Wants=network-online.target
# After=network-online.target
#
# [Service]
# User=octoprint
# ExecStart=/usr/local/sbin/ssh-tunnel.sh
# Restart=always
#
# [Install]
# WantedBy=multi-user.target

#
# ========================================== DOCUMENTATION ======================================= }}}
# Variables
LISTEN_ADDRESS="localhost"
LOCAL_PORT="22"
REMOTE_PORT="9444"
REMOTE_USER="gilean"
REMOTE_HOST="smilla.karlsbakk.net"
SSH_FLAGS="-v -o ServerAliveInterval=20 -o ServerAliveCountMax=5 -o ExitOnForwardFailure=yes -n -N -R $REMOTE_PORT:$LISTEN_ADDRESS:$LOCAL_PORT $REMOTE_USER@$REMOTE_HOST"
SSH_CMD="/usr/bin/ssh"

$SSH_CMD $SSH_FLAGS

