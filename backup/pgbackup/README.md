[comment]: <> vim:tw=80:ts=4:sw=4:sts=4:et:ai
# Simple PostgreSQL backup

A simple PostgreSQL backup script meant to be used with logrotate.

## Installation

Copy the `pg-dump-all-databases.sh` file to a suitable location. In this
example, under the postgres user's homedir. On Debian and many other distros,
this will be /var/lib/postgresql. To make things bit tidier, make a bin dir
under this, so /var/lib/postgresql/bin. Make sure the file is executable (`chmod
+x pg-dump-all-databases.sh`).

### cron

Add a crontab entry for the user postgres with `crontab -e -u postgres` with the
following line

`35 2 * * *      $HOME/bin/pg-dump-all-databases.sh`

### logrotate

Configurea logrotate to rotate all files under the default target dir (by
default, /var/lib/postgresql/backups). On my setup, I've done thie with system
logrotate, but using a separate file and running it as a user process should be
similar. I've added `/etc/logrotate.d/postgresql-backups` with the following
contents

    # vim:ts=4:sw=4:sts=4:et:ai
    /var/lib/postgresql/backups/*.dump {
        daily
        rotate 10
        copytruncate
        nocompress
        notifempty
        nocreate
        su postgres postgres
    }

This will rotate the logs daily at the time given in cron on when logrotate is
run. On my system, this is defined in /etc/crontab, trigging cron-daily at
06:47. As shown above, postgresql is backed up at 02:35, so some time between
the two.

Also, note the **nocompress** statement in the lograte config. The default is to
compress rotated files (typically logs, hence the name of the program), but as
the backup script uses the *custom* format for backups, these are already
gzipped internally, so no need for more compression.

[roy](mailto:roy@karlsbakk.net)
