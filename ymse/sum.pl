#!/usr/bin/perl
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker
#
# Example input from df -kt
"""
Filesystem                 Type     1K-blocks      Used Available Use% Mounted on
udev                       devtmpfs  65769420         0  65769420   0% /dev
tmpfs                      tmpfs     13165184      3496  13161688   1% /run
/dev/mapper/vg00-root      ext4       4886152   3879444    751124  84% /
tmpfs                      tmpfs     65825920        16  65825904   1% /dev/shm
tmpfs                      tmpfs         5120         0      5120   0% /run/lock
/dev/sda2                  ext4        965872    150772    748696  17% /boot
/dev/mapper/vg00-home      ext4        964900        24    898524   1% /home
/dev/mapper/vg00-pcap      ext4       3021608        56   2847884   1% /root/pcap
/dev/mapper/vg00-opt       ext4        964900    701692    196856  79% /opt
/dev/mapper/vg00-src       ext4      10218772   2123828   8078560  21% /root/src
/dev/mapper/vg00-var       ext4       1928840   1013240    799276  56% /var
/dev/sda1                  vfat        523244      5940    517304   2% /boot/efi
/dev/mapper/vg00-backups   ext4       4046560     24832   3795632   1% /var/backups
/dev/mapper/vg00-log       ext4       8154588   1333496   6385280  18% /var/log
/dev/mapper/vg00-postgres  ext4     308522624 278784352  29721888  91% /var/lib/postgresql
/dev/mapper/vg00-tmp       ext4       2046456       216   1937948   1% /tmp
/dev/mapper/vg00-pgtmp     ext4      10022200   3773336   5758536  40% /var/lib/postgresql/tmp
/dev/mapper/vg00-pgbackups ext4     207133664  31802360 175314920  16% /var/lib/postgresql/backups
tmpfs                      tmpfs     13165184         0  13165184   0% /run/user/0
"""

my $needsroot = 0;

if ($needsroot) {
    my $current_user = getpwuid($<);
    die "This needs to be run as root, not by user $current_user (UID $<)" if ( $< != 0 );
}

