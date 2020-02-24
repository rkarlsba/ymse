#!/usr/bin/env python3
# vim:ts=4:sw=4:sts=4:et:ai

import configparser
import syslog
from time import sleep
import sys

# globals
config_file = "pi-tempctl.conf"
temperature_file = "/sys/class/thermal/thermal_zone0/temp";
fan_status = 0
probe_count = 0

# defaults
c_temperaure_threashold = 45
c_probe_count = 1
c_fan_pin = 0;
c_poll_delay = 1;

def gettemp():
    try:
        tempf = open(temperature_file, "r")
        mc = float(tempf.read())
        tempf.close()
        return mc/1000.0
    except (RuntimeError, TypeError, NameError):
        print("Some bloody error")

def fanctl(status):
    if status == 0:
        print("Turn fan off")
        syslog.syslog(syslog.LOG_INFO, "Fan off");
    else:
        print("Turn fan on")
        syslog.syslog(syslog.LOG_INFO, "Fan on");

syslog.openlog("pi-tempctl.py", logoption=syslog.LOG_PID, facility=syslog.LOG_DAEMON)

# read config
config = configparser.ConfigParser()
config.read(config_file)
for key in config["default"]:
    if key == "temperaure_threashold":
        c_temperaure_threashold = config.getfloat("default","temperaure_threashold");
    elif key == "probe_count":
        c_probe_count = config.getint("default","probe_count");
    elif key == "fan_pin":
        c_fan_pin = config.getint("default","fan_pin");
    elif key == "poll_delay":
        c_poll_delay = config.getfloat("default","poll_delay");

syslog_base_message = "DEBUG: Got temperaure_threashold {}, probe_count {}, fan_pin {} and poll_delay {}";
syslog_message = syslog_base_message.format(c_temperaure_threashold, c_probe_count, c_fan_pin, c_poll_delay);
syslog.syslog(syslog.LOG_DEBUG, syslog_message);

while 1:
    temp = gettemp()
    if fan_status == 0: # fan status is off
        if temp > c_temperaure_threashold:
            probe_count+=1
            if probe_count >= c_probe_count:
                probe_count=0
                fanctl(1)
                fan_status=1
    else: # fan status is on
        if temp <= c_temperaure_threashold:
            probe_count+=1
            if probe_count >= c_probe_count:
                probe_count=0
                fanctl(0)
                fan_status=0
            
    print("Test {0:03d} (sleep {1:f}). Temp is {2} and fan status is {3}".format(probe_count,c_poll_delay,temp,fan_status))
    sleep(c_poll_delay)

