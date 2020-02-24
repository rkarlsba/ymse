#!/usr/bin/env python3
# vim:ts=4:sw=4:sts=4:et:ai

import configparser
import syslog
from time import sleep

# globals
config_file = 'pi-tempctl.conf'

# defaults
c_temperaure_threashold = 45
c_hysteresis_avoidance = 1
c_fan_pin = 0;
c_poll_delay = 1;

syslog.openlog("pi-tempctl.py", logoption=syslog.LOG_PID, facility=syslog.LOG_DAEMON)

# read config
config = configparser.ConfigParser()
config.read(config_file)
for key in config['default']:
    if key == 'temperaure_threashold':
        c_temperaure_threashold = config['default']['temperaure_threashold'];
    elif key == 'hysteresis_avoidance':
        c_hysteresis_avoidance = config['default']['hysteresis_avoidance'];
    elif key == 'fan_pin':
        c_fan_pin = config['default']['fan_pin'];
    elif key == 'poll_delay':
        c_poll_delay = config.getint['default']['poll_delay'];

syslog_base_message = "DEBUG: Got temperaure_threashold {}, hysteresis_avoidance {}, fan_pin {} and poll_delay {}";
syslog_message = syslog_base_message.format(c_temperaure_threashold, c_hysteresis_avoidance, c_fan_pin, c_poll_delay);
syslog.syslog(syslog.LOG_DEBUG, syslog_message);

count = 0
while 1:
    print("Test {0:03d} (sleep {1:f})".format(count,c_poll_delay))
    count=count+1
    sleep(c_poll_delay)
