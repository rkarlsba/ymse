# Restic backup script documentation

Set ssh user, hostname and port in $HOME/.ssh/config - it should look
something like this (without the initial #). See man ssh_config(5) for more
details.

```
host restic
 	Hostname restic.my.tld
#	AddressFamily inet
#	Port 3422
 	User mysshusername
```

The exclude file referred to below must exist if you're not using
--one-file-system. It should include at least these four lines. It's installed
by default under /usr/local/etc unless the file exists already.

```
/proc
/sys
/dev
/run
```

Written by [mailto:roy@karlsbakk.net](Roy Sigurd Karlsbakk)
