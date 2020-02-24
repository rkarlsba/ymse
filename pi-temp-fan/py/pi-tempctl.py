#!/usr/bin/env python3
# vim:ts=4:sw=4:sts=4:et:ai

import configparser
import syslog

# globals
config_file = 'pi-tempctl.conf'

# defaults
c_temperaure_threashold = 45
c_hysteresis_avoidance = 1
c_fan_pin = 0;
c_poll_rate = 1;

syslog.openlog("pi-tempctl.py", logoption=syslog.LOG_PID, facility=syslog.LOG_DAEMON)

# read config
config = configparser.ConfigParser()
config.read(config_file)
for key in config['default']:
    if key == 'temperaure_threashold':
        c_temperaure_threashold = config['default']['temperaure_threashold'];
    elif key == 'hysteresis_avoidance':
        c_hysteresis_avoidance = config['default']['hysteresis_avoidance'];

syslog_message = "DEBUG: Got temperaure_threashold {} and hysteresis_avoidance {}".format(c_temperaure_threashold, c_hysteresis_avoidance);
syslog.syslog(syslog.LOG_DEBUG, syslog_message);


