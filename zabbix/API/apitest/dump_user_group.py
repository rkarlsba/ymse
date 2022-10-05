#!/usr/bin/env python3
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker

from pyzabbix.api import ZabbixAPI, ZabbixAPIException
import json
import sys
from pprint import pprint

# Globals
verbose = 0

try:
    from local_passwords import *
except:
    print("File local_passwords.py does not exists. Please refer to the README.md file", file=sys.stderr)
    print("and create the named file before running this again.", file=sys.stderr)
    exit(1)

try:
    # Create ZabbixAPI class instance
    zapi = ZabbixAPI(url=api_url, user=api_user, password=api_password)

    try:
        '''
        01  action
        02  alert
        03  apiinfo
        04  auditlog
        05  authentication
        06  autoregistration
        07  configuration
        08  correlation
        09  dashboard
        10  dhost
        11  dservice
        12  dcheck
        13  drule
        14  event
        15  graph
        16  graphitem
        17  graphprototype
        18  hanode
        19  history
        20  host
        21  hostgroup
        22  hostinterface
        23  hostprototype
        24  housekeeping
        25  iconmap
        26  image
        27  item
        28  itemprototype
        29  discoveryrule
        30  maintenance
        31  map
        32  mediatype
        33  problem
        34  proxy
        35  regexp
        36  report
        37  role
        38  script
        39  service
        40  settings
        41  sla
        42  task
        43  template
        44  templatedashboard
        45  templategroup
        46  token
        47  trend
        48  trigger
        49  triggerprototype
        50  user
        51  userdirectory
        52  usergroup
        53  usermacro           <-- ??
        54  valuemap
        55  httptest            <-- web scenarios
        '''
        # Get all hostgroups
        hostgroups = json.dumps(zapi.hostgroup.get(output='extend'))
        with open('data/hostgroups.json', 'w') as f:
            print(hostgroups, file=f)
        f.close;

        # Get all hostinterfaces
        hostinterfaces = json.dumps(zapi.hostinterface.get(output='extend'))
        with open('data/hostinterfaces.json', 'w') as f:
            print(hostinterfaces, file=f)

        # Get all users
        allusers = json.dumps(zapi.user.get(output='extend'))
        with open('data/users.json', 'w') as f:
            print(allusers, file=f)

        # Get all usergroups
        allusergroups = json.dumps(zapi.usergroup.get(output='extend'))
        with open('data/usergroups.json', 'w') as f:
            print(allusergroups, file=f)

    except:
        print("Something wierd happened: $?\n", file=sy.stderr);
        exit(2)

    zapi.user.logout()

except ZabbixAPIException as e:
    print("Some Zabbix API error: {}".format(e))

