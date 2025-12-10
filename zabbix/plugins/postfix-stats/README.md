<!--
vim:ts=4:sw=4:sts=4:et:ai:fdm=marker:tw=80
-->

# zabbix-postfix-template
Zabbix template for Postfix SMTP server

Works for Zabbix 4.x

Forked from http://admin.shamot.cz/?p=424

# Requirements
* [pflogsum](http://jimsun.linxnet.com/postfix_contrib.html)
* [pygtail](https://pypi.org/project/pygtail/)

# Installation
    # for Ubuntu / Debian
    apt-get install pflogsumm pygtail
    
    # for CentOS
    yum install postfix-perl-scripts
    cp pygtail.py /usr/bin/
    chmod +x /usr/bin/pygtail.py
    
    # ! check MAILLOG path in zabbix-postfix-stats.sh
    cp zabbix-postfix-stats.sh /usr/bin/
    chmod +x /usr/bin/zabbix-postfix-stats.sh

    cp userparameter_postfix.conf /etc/zabbix/zabbix_agentd.d/

    # Change the group of the maillog
    On debian and related systems, it's called /var/log/mail.log and
    /var/log/maillog on RHEL and its likes. This file is normally listed in
    /etc/logrotate.d/rsyslog and should look something like this (this is from a
    Debian machine)
    
        /var/log/syslog
        /var/log/mail.info
        /var/log/mail.log
        /var/log/mail.warn
        /var/log/mail.err
        /var/log/daemon.log
        /var/log/kern.log
        /var/log/auth.log
        /var/log/user.log
        /var/log/lpr.log
        /var/log/cron.log
        /var/log/debug
        /var/log/messages
        {
                rotate 4
                weekly
                missingok
                notifempty
                compress
                delaycompress
                sharedscripts
                postrotate
                        /usr/lib/rsyslog/rsyslog-rotate
                endscript
        }

    Now, keep the above, minus the line with mail.log and pull that out and give
    it a separate section, something like this. Please note that the only
    difference between this and the above, is the **create** line, which tells
    logrotate the mode and ownership of the new file.

    /var/log/mail.log
    {
            rotate 4
            weekly
            missingok
            notifempty
            compress
            delaycompress
            sharedscripts
            create 0640 root zabbix
            postrotate
                    /usr/lib/rsyslog/rsyslog-rotate
            endscript
    }

    Now, since this only changes the file when it's rotated, change the current
    file's owner and mode manually:

    # chown :zabbix /var/log/mail.log
    # chmod g+r /var/log/mail.log

Finally import template_app_zabbix.xml and attach it to your host. You may want
to change the macros holding the thresholds 

- {$POSTFIX_MAX_BOUNCED}
- {$POSTFIX_MAX_DEFERRED}
- {$POSTFIX_MAX_HELD}
- {$POSTFIX_MAX_REJECTED}
- {$POSTFIX_MAX_REJECT_WARNINGS}

The busier the server is and the more pentests and such you run against it, the
more rejects and the likes you'll get, so better tune these values to meet your
needs.

Now, just add a file to configure the zabbix agent, perhaps named it
userparameter_postfix.conf and place it under /etc/zabbix/zabbix_agent2.d/ or
whatever zabbix agent you're using. The file should contain the following:

    UserParameter=postfix.pfmailq,mailq | grep -v "Mail queue is empty" | grep -c '^[0-9A-Z]'
    UserParameter=postfix[*],/usr/bin/zabbix-postfix-stats.sh $1
    UserParameter=postfix.update_data,/usr/bin/zabbix-postfix-stats.sh

Test well and pray to your favourite god(s) and please don't blame me if it all
goes bad.

[roy](mailto:roy@karlsbakk.net)
[roy](mailto:roysk@oslomet.no)
