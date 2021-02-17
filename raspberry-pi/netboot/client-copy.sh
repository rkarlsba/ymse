#!/bin/bash

name=$1
template="/dev/nfsroot/_template_"
snapsize="2G"
mountpoint="/dev/nfsroot/$name"

echo "Creating new client '$name'"
echo "Finish testing first..." >&2
exit 1
lvcreate --size $snapsize --snapshot --name $name $template
echo "/dev/nfsroot/whitepi	/nfsroot/whitepi	ext4	errors=remount-ro	0	2" >> /etc/fstab
mount -a
echo "/nfsroot/whitepi        192.168.10.168(rw,sync,no_subtree_check,no_root_squash)" >> /etc/exports
