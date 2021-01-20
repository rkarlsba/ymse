# zabbix_linux_distro_check.pl

Small script to extract linux distro info to zabbix items for later use.

To use, copy zabbix_linux_distro_check.pl to an appropriate place, such as /usr/local/bin,
chmod +x it and add the following lines to your agent config:

```
# Add these to the config somewhere
UserParameter=custom.distro.name,/usr/local/bin/zabbix_check_distro.pl --name
UserParameter=custom.distro.version,/usr/local/bin/zabbix_check_distro.pl --vers
UserParameter=custom.distro.arch,/usr/local/bin/zabbix_check_distro.pl --arch
UserParameter=custom.distro.friendlyname,/usr/local/bin/zabbix_check_distro.pl --friendly
```

Written by [Roy Sigurd Karlsbakk](mailto:roy@karlsbakk.net]).

Licensed under AGPLv3. Please see the LICENSE document in this directory or
[read it online here](https://www.gnu.org/licenses/agpl-3.0.en.html
