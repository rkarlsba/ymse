<!--
vim:ts=4:sw=4:sts=4:et:ai:fdm=marker:tw=80
-->
# Rewrite required

After my rewrite of this to not needing sudo, a rewrite of this documentation is
required, since it currently doesn't make sense. Also, either the docs or the
script itself needs to check which maillog to check. In the file
/etc/rsyslog.conf, there should be something like this on Debian

    mail.*				-/var/log/mail.log

and this on RHEL/CentOS

    mail.*              -/var/log/maillog

So this should extract the filename easily

    $ perl -ne 'print "$1\n" if (/^mail\..*?(\/.*)/)' /etc/rsyslog.conf

[roy](mailto:roy@karlsbakk.net) December 2025

# zabbix-postfix-template
Zabbix template for Postfix SMTP server

Works for Zabbix 4.x

Forked from http://admin.shamot.cz/?p=424

# Requirements
* [pflogsum](http://jimsun.linxnet.com/postfix_contrib.html)
* [pygtail](https://pypi.org/project/pygtail/)

# Installation
    # for Ubuntu / Debian
    apt-get install pflogsumm
    
    # for CentOS
    yum install postfix-perl-scripts
    
    cp pygtail.py /usr/sbin/
    chmod +x /usr/sbin/pygtail.py
    
    # ! check MAILLOG path in zabbix-postfix-stats.sh
    cp zabbix-postfix-stats.sh /usr/bin/
    chmod +x /usr/bin/zabbix-postfix-stats.sh

    cp userparameter_postfix.conf /etc/zabbix/zabbix_agentd.d/
    
    # run visudo as root
    Defaults:zabbix !requiretty
    zabbix ALL=(ALL) NOPASSWD: /usr/bin/zabbix-postfix-stats.sh
    
    systemctl restart zabbix-agent

Finally import template_app_zabbix.xml and attach it to your host
