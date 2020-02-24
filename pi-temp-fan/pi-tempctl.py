#!/usr/bin/env python3
# vim:ts=4:sw=4:sts=4:et:ai

import configparser

# globals
config_file = 'pi-tempctl.conf'

# defaults
c_temperaure_threashold = 45
c_hysteresis_avoidance = 1

# read config
config = configparser.ConfigParser()
config.read(config_file)
for key in config['default']:
    if key == 'temperaure_threashold':
        c_temperaure_threashold = config['default']['temperaure_threashold'];
    elif key == 'hysteresis_avoidance':
        c_hysteresis_avoidance = config['default']['hysteresis_avoidance'];
print("Got temperaure_threashold {} and hysteresis_avoidance {}".format(c_temperaure_threashold, c_hysteresis_avoidance));
