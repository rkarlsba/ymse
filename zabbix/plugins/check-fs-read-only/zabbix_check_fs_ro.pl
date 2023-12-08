#!/usr/bin/perl
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker

# Slik ser det ut i fila vi les frå {{{
# sysfs /sys sysfs rw,nosuid,nodev,noexec,relatime 0 0
# proc /proc proc rw,nosuid,nodev,noexec,relatime 0 0
# devtmpfs /dev devtmpfs rw,nosuid,size=929356k,nr_inodes=232339,mode=755 0 0
# securityfs /sys/kernel/security securityfs rw,nosuid,nodev,noexec,relatime 0 0
# tmpfs /dev/shm tmpfs rw,nosuid,nodev 0 0
# devpts /dev/pts devpts rw,nosuid,noexec,relatime,gid=5,mode=620,ptmxmode=000 0 0
# tmpfs /run tmpfs rw,nosuid,nodev,mode=755 0 0
# tmpfs /sys/fs/cgroup tmpfs ro,nosuid,nodev,noexec,mode=755 0 0
# cgroup /sys/fs/cgroup/systemd cgroup rw,nosuid,nodev,noexec,relatime,xattr,release_agent=/usr/lib/systemd/systemd-cgroups-agent,name=systemd 0 0
# pstore /sys/fs/pstore pstore rw,nosuid,nodev,noexec,relatime 0 0
# cgroup /sys/fs/cgroup/hugetlb cgroup rw,nosuid,nodev,noexec,relatime,hugetlb 0 0
# cgroup /sys/fs/cgroup/freezer cgroup rw,nosuid,nodev,noexec,relatime,freezer 0 0
# cgroup /sys/fs/cgroup/perf_event cgroup rw,nosuid,nodev,noexec,relatime,perf_event 0 0
# cgroup /sys/fs/cgroup/net_cls,net_prio cgroup rw,nosuid,nodev,noexec,relatime,net_prio,net_cls 0 0
# cgroup /sys/fs/cgroup/cpu,cpuacct cgroup rw,nosuid,nodev,noexec,relatime,cpuacct,cpu 0 0
# cgroup /sys/fs/cgroup/pids cgroup rw,nosuid,nodev,noexec,relatime,pids 0 0
# cgroup /sys/fs/cgroup/devices cgroup rw,nosuid,nodev,noexec,relatime,devices 0 0
# cgroup /sys/fs/cgroup/cpuset cgroup rw,nosuid,nodev,noexec,relatime,cpuset 0 0
# cgroup /sys/fs/cgroup/memory cgroup rw,nosuid,nodev,noexec,relatime,memory 0 0
# cgroup /sys/fs/cgroup/blkio cgroup rw,nosuid,nodev,noexec,relatime,blkio 0 0
# configfs /sys/kernel/config configfs rw,relatime 0 0
# /dev/mapper/rhel_dhcp--158--36--161--249-root / xfs rw,relatime,attr2,inode64,noquota 0 0
# systemd-1 /proc/sys/fs/binfmt_misc autofs rw,relatime,fd=26,pgrp=1,timeout=300,minproto=5,maxproto=5,direct 0 0
# sunrpc /var/lib/nfs/rpc_pipefs rpc_pipefs rw,relatime 0 0
# hugetlbfs /dev/hugepages hugetlbfs rw,relatime 0 0
# debugfs /sys/kernel/debug debugfs rw,relatime 0 0
# mqueue /dev/mqueue mqueue rw,relatime 0 0
# nfsd /proc/fs/nfsd nfsd rw,relatime 0 0
# /dev/sda1 /boot xfs rw,relatime,attr2,inode64,noquota 0 0
# /dev/mapper/vgstudfelles-lvstudfelles /data ext4 rw,relatime,data=ordered,jqfmt=vfsv0,usrjquota=aquota.user 0 0
# tmpfs /run/user/0 tmpfs rw,nosuid,nodev,relatime,size=187996k,mode=700 0 0
# /dev/loop0 /root/x ext4 ro,relatime,data=ordered 0 0
# }}}
# Dokumentasjon {{{
#
# Skriptet går gjennom /proc/mounts og ser etter filsystemer montert read-only,
# siden det er noe vi generelt ikke vil ha. For noen filsystemer og/eller
# monteringspunkter, er dette imidlertid vanlig,som i tilfellet /sys/fs/cgroup
# og av og til /boot/efi. Skriptet inneholder derfor variabler for dette slik at
# disse kan ekskluderes for å unngå falske positiver.
#
# Merk at dette skriptet ser på maskinen globalt, mens zabbix_check_fs_ro_lld.pl
# er der for å legge til såkalt "low level discovery" for filsystemene som ikke
# er ekskludert, og vil vise read-only-status direkte for filsystemet i stedet
# for vertsmaskinen, som dette gjør. Bruk heller nevnte skript enn dette, med
# mindre du har spesielle behov.
#
# Roy Sigurd Karlsbakk <roysk@oslomet.no> <roy@karlsbakk.net>
#
# }}}

use strict;
use warnings;

my %ignored_dirs = (
    '/sys/fs/cgroup'    => 1,
    '/boot/efi'         => 1,
);
my @rofses;

open MOUNTS,'/proc/mounts';
while (my $m = <MOUNTS>) {
    my @cols = split(/\s+/, $m);
    next if ($ignored_dirs{$cols[1]});

    my @fsopts = split(/,/, $cols[3]);
    my $fsro = 0;
    foreach my $fsopt (@fsopts) {
        if ($fsopt eq 'ro') {
            push(@rofses, $cols[0]);
            $fsro++;
            last;
        }
    }
}
close MOUNTS;
my $fses = $#rofses+1;

if ($fses > 0) {
    my $s = '';
    $s = 's' if ($fses > 1);
    print "WARNING: Found $fses filesystem$s mounted read-only: ";
    foreach my $fs (@rofses) {
        print "$fs ";
    }
    print "\n";
    exit $fses;
} else {
    print "OK: All filesystems mounted read-write\n";
}
