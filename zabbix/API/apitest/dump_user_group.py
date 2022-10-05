#!/usr/bin/env python3
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker

from pyzabbix.api import ZabbixAPI, ZabbixAPIException
import json
import sys
from pprint import pprint
import traceback
import logging

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
        07  configuration       <-- only 'export' and 'import' methods
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
        20  host                <-- 604 av disse per 2022-10-06
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
        39  service             <-- Tomt på 5.x
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
        51  userdirectory       <-- Finnes først fra 6.x
        52  usergroup
        53  usermacro           <-- ??
        54  valuemap
        55  httptest            <-- web scenarios - märklich - ser ut til at det ligger 259 av disse der - littegranne mange, kanskje?
        '''
        # Get all dashboards (09)
        dashboard = json.dumps(zapi.dashboard.get(output='extend'))
        with open('data/dashboard.json', 'w') as f:
            print(dashboard, file=f)
        f.close;

        # Get all hosts (21)
        host = json.dumps(zapi.host.get(output='extend'))
        with open('data/host.json', 'w') as f:
            print(host, file=f)
        f.close;

        # Get all hostgroups (21)
        hostgroup = json.dumps(zapi.hostgroup.get(output='extend'))
        with open('data/hostgroup.json', 'w') as f:
            print(hostgroup, file=f)
        f.close;

        # Get all hostinterfaces (22)
        hostinterface = json.dumps(zapi.hostinterface.get(output='extend'))
        with open('data/hostinterface.json', 'w') as f:
            print(hostinterface, file=f)
        f.close;

        # Get all hostprototypes (23)
        hostprototype = json.dumps(zapi.hostprototype.get(output='extend'))
        with open('data/hostprototype.json', 'w') as f:
            print(hostprototype, file=f)
        f.close;

        # # Get all services (39)
        # service = json.dumps(zapi.service.get(output='extend'))
        # with open('data/service.json', 'w') as f:
        #     print(service, file=f)
        # f.close;

        # Get all users (50)
        user = json.dumps(zapi.user.get(output='extend'))
        with open('data/user.json', 'w') as f:
            print(user, file=f)
        f.close;

        # Get all usergroups (52)
        usergroup = json.dumps(zapi.usergroup.get(output='extend'))
        with open('data/usergroup.json', 'w') as f:
            print(usergroup, file=f)
        f.close;

        # Get all httptest (55) - web scenarias
        httptest = json.dumps(zapi.httptest.get(output='extend'))
        with open('data/httptest.json', 'w') as f:
            print(httptest, file=f)
        f.close;

    except Exception as e:
        print("Something wierd happened: $?\n", file=sys.stderr);
        logging.error(traceback.format_exc())
        exit(2)

    zapi.user.logout()

except ZabbixAPIException as e:
    print("Some Zabbix API error: {}".format(e))

