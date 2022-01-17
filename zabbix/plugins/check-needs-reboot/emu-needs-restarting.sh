#!/bin/bash
# vim:ts=4:sw=4:sts=4:et:ai

if [ "$1" == "-r" ]
then
	echo "Modular dependency problems:

 Problem 1: conflicting requests
  - nothing provides module(perl:5.26) needed by module perl-DBD-MySQL:4.046:8010020191114030811:073fa5fe-0.x86_64
 Problem 2: conflicting requests
  - nothing provides module(perl:5.26) needed by module perl-DBI:1.641:8010020191113222731:16b3ab4d-0.x86_64
 Problem 3: conflicting requests
  - nothing provides module(perl:5.26) needed by module perl-IO-Socket-SSL:2.066:8030020201222215140:1e4bbb35-0.x86_64
 Problem 4: conflicting requests
  - nothing provides module(perl:5.26) needed by module perl-libwww-perl:6.34:8030020201223164340:b967a9a2-0.x86_64" >&2
    echo "Core libraries or services have been updated since boot-up:
  * dbus
  * dbus-daemon
  * glibc
  * kernel
  * linux-firmware
  * systemd

Reboot is required to fully utilize these updates.
More information: https://access.redhat.com/solutions/27943"
else
    echo "Modular dependency problems:

 Problem 1: conflicting requests
  - nothing provides module(perl:5.26) needed by module perl-DBD-MySQL:4.046:8010020191114030811:073fa5fe-0.x86_64
 Problem 2: conflicting requests
  - nothing provides module(perl:5.26) needed by module perl-DBI:1.641:8010020191113222731:16b3ab4d-0.x86_64
 Problem 3: conflicting requests
  - nothing provides module(perl:5.26) needed by module perl-IO-Socket-SSL:2.066:8030020201222215140:1e4bbb35-0.x86_64
 Problem 4: conflicting requests
  - nothing provides module(perl:5.26) needed by module perl-libwww-perl:6.34:8030020201223164340:b967a9a2-0.x86_64" >&2
    echo "1 : /usr/lib/systemd/systemd --system --deserialize 20
877 : /usr/lib/systemd/systemd-journald
1116 : /usr/bin/lsmd -d
1120 : /usr/sbin/irqbalance --foreground
1131 : /usr/bin/dbus-daemon --system --address=systemd: --nofork --nopidfile --systemd-activation --syslog-only
1132 : /usr/sbin/chronyd
1134 : /usr/sbin/NetworkManager --no-daemon
1317 : /usr/lib/systemd/systemd-logind
1323 : /usr/sbin/atd -f
1328 : /sbin/agetty -o -p -- \u --noclear tty1 linux
1583 : /usr/lib/systemd/systemd-resolved
4914 : tmux
4915 : -bash
6075 : /usr/sbin/crond -n
23040 : /usr/lib/systemd/systemd --user
23043 : (sd-pam)"
fi

exit 1
