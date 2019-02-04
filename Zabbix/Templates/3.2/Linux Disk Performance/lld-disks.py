#!/usr/bin/python
import os
import json
import re

if __name__ == "__main__":
    # filter out partitions
    rx_sd = re.compile('^sd[a-z]+$')
    # Iterate over all block devices, but ignore them if they are in the
    # skippable set
    skippable = ("sr", "loop", "ram")
    devices = (device for device in os.listdir("/sys/class/block")
               if not any(ignore in device for ignore in skippable)
               and rx_sd.match(device))
    data = [{"{#DEVICENAME}": device} for device in devices]
    print(json.dumps({"data": data}, indent=4))
