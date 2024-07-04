#!/usr/bin/perl
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker

use strict;
use warnings;

my $etc_fstab = '/etc/fstab';
my $proc_mounts = '/proc/mounts';

# Example /etc/fstab {{{
#
# # <file system>                         <mount point>   <type>  <options>           <dump>  <pass>
# /dev/mapper/sys-root                    /               ext4    errors=remount-ro   0       1
# /dev/mapper/sys-var                     /var            ext4    errors=remount-ro   0       2
# /dev/mapper/sys-tmp                     /tmp            ext4    errors=remount-ro   0       2
# 
# # NFS 
# smilla:/data/porn                       /restic         nfs     defaults            0       0
# smilla:/data/empetre                    /mnt/empetre    nfs     defaults            0       0
# 
# https://webdav.server.com/somewhere     /mnt/webdav     davfs   user,rw,auto        0       0
# 
# # Swappetiswap
# /dev/sys/swap                           none            swap    sw                  0       0
#
# }}}
# Example /proc/mounts {{{
#
# sysfs /sys sysfs rw,nosuid,nodev,noexec,relatime 0 0
# proc /proc proc rw,nosuid,nodev,noexec,relatime 0 0
# udev /dev devtmpfs rw,nosuid,relatime,size=8054332k,nr_inodes=2013583,mode=755,inode64 0 0
# devpts /dev/pts devpts rw,nosuid,noexec,relatime,gid=5,mode=620,ptmxmode=000 0 0
# tmpfs /run tmpfs rw,nosuid,nodev,noexec,relatime,size=1626796k,mode=755,inode64 0 0
# /dev/mapper/sys-root / ext4 rw,relatime,errors=remount-ro 0 0
# securityfs /sys/kernel/security securityfs rw,nosuid,nodev,noexec,relatime 0 0
# tmpfs /dev/shm tmpfs rw,nosuid,nodev,inode64 0 0
# tmpfs /run/lock tmpfs rw,nosuid,nodev,noexec,relatime,size=5120k,inode64 0 0
# cgroup2 /sys/fs/cgroup cgroup2 rw,nosuid,nodev,noexec,relatime,nsdelegate,memory_recursiveprot 0 0
# pstore /sys/fs/pstore pstore rw,nosuid,nodev,noexec,relatime 0 0
# bpf /sys/fs/bpf bpf rw,nosuid,nodev,noexec,relatime,mode=700 0 0
# systemd-1 /proc/sys/fs/binfmt_misc autofs rw,relatime,fd=30,pgrp=1,timeout=0,minproto=5,maxproto=5,direct,pipe_ino=18747 0 0
# mqueue /dev/mqueue mqueue rw,nosuid,nodev,noexec,relatime 0 0
# hugetlbfs /dev/hugepages hugetlbfs rw,relatime,pagesize=2M 0 0
# debugfs /sys/kernel/debug debugfs rw,nosuid,nodev,noexec,relatime 0 0
# tracefs /sys/kernel/tracing tracefs rw,nosuid,nodev,noexec,relatime 0 0
# fusectl /sys/fs/fuse/connections fusectl rw,nosuid,nodev,noexec,relatime 0 0
# configfs /sys/kernel/config configfs rw,nosuid,nodev,noexec,relatime 0 0
# ramfs /run/credentials/systemd-sysusers.service ramfs ro,nosuid,nodev,noexec,relatime,mode=700 0 0
# ramfs /run/credentials/systemd-tmpfiles-setup-dev.service ramfs ro,nosuid,nodev,noexec,relatime,mode=700 0 0
# nfsd /proc/fs/nfsd nfsd rw,relatime 0 0
# ramfs /run/credentials/systemd-sysctl.service ramfs ro,nosuid,nodev,noexec,relatime,mode=700 0 0
# /dev/mapper/arkiv-quotatest /quotatest xfs rw,relatime,attr2,inode64,logbufs=8,logbsize=32k,sunit=2048,swidth=4096,usrquota 0 0
# /dev/mapper/arkiv-arkiv /arkiv xfs rw,relatime,attr2,inode64,logbufs=8,logbsize=32k,sunit=2048,swidth=2048,noquota 0 0
# /dev/mapper/arkiv-arkiv /data xfs rw,relatime,attr2,inode64,logbufs=8,logbsize=32k,sunit=2048,swidth=2048,noquota 0 0
# /dev/mapper/arkiv-arkiv /var/lib/libvirt/images xfs rw,relatime,attr2,inode64,logbufs=8,logbsize=32k,sunit=2048,swidth=2048,noquota 0 0
# /dev/mapper/sys-kvm_ssd /var/lib/libvirt/images/ssd ext4 rw,relatime,errors=remount-ro 0 0
# /dev/mapper/sys-kvm_ssd /arkiv/kvm/ssd ext4 rw,relatime,errors=remount-ro 0 0
# /dev/mapper/sys-kvm_ssd /data/kvm/ssd ext4 rw,relatime,errors=remount-ro 0 0
# ramfs /run/credentials/systemd-tmpfiles-setup.service ramfs ro,nosuid,nodev,noexec,relatime,mode=700 0 0
# binfmt_misc /proc/sys/fs/binfmt_misc binfmt_misc rw,nosuid,nodev,noexec,relatime 0 0
# /dev/mapper/arkiv-tmp /arkiv/tmp ext4 rw,relatime,errors=remount-ro,stripe=512 0 0
# /dev/mapper/arkiv-tmp /data/tmp ext4 rw,relatime,errors=remount-ro,stripe=512 0 0
# sunrpc /run/rpc_pipefs rpc_pipefs rw,relatime 0 0
# tmpfs /run/user/1024 tmpfs rw,nosuid,nodev,relatime,size=1626792k,nr_inodes=406698,mode=700,uid=1024,gid=1024,inode64 0 0
# tmpfs /run/user/1000 tmpfs rw,nosuid,nodev,relatime,size=1626792k,nr_inodes=406698,mode=700,uid=1000,gid=1000,inode64 0 0
# tmpfs /run/user/0 tmpfs rw,nosuid,nodev,relatime,size=1626792k,nr_inodes=406698,mode=700,inode64 0 0
#
# }}}


