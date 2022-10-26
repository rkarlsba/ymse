#!/usr/bin/env python3
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker

import json
import sys
from pprint import pprint
import traceback
import logging

try:
    from pyzabbix.api import ZabbixAPI, ZabbixAPIException
except:
    print("You'll need ZabbixAPI and its friends to make any use of these tools [1]", file=sys.stderr)
    exit(1)

try:
    from local_passwords import *
except:
    print("File local_passwords.py does not exists. Please refer to the README.md file", file=sys.stderr)
    print("and create the named file before running this again.", file=sys.stderr)
    exit(1)

# Globals
verbose = 0

try:
    # Create ZabbixAPI class instance
    zapi = ZabbixAPI(url=api_dst_url, user=api_dst_user, password=api_dst_password)

    try:
        ''' objektliste {{{
        01  action
        02  alert
        03  apiinfo
        03  application
        04  auditlog
        05  authentication
        06  autoregistration
        07  configuration       <-- only 'export' and 'import' methods
        08  correlation         <-- Empty on 5.0
        09  dashboard           <-- Imported
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
        39  service             <-- Empty on 5.0
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
 }}}    '''
        # Get all usergroups in dump (52)
        '''
[
    {
        "usrgrpid": "7",
        "name": "Zabbix administrators",
        "gui_access": "0",
        "users_status": "0",
        "debug_mode": "0"
    },
    {
        "usrgrpid": "9",
        "name": "Disabled",
        "gui_access": "0",
        "users_status": "1",
        "debug_mode": "0"
    },
    {
        "usrgrpid": "12",
        "name": "No access to the frontend",
        "gui_access": "3",
        "users_status": "0",
        "debug_mode": "0"
        '''
        with open('data/usergroup.json', 'r') as usergroups_f:
            # Print the type of data variable
            # print("Type:", type(usergroups))
            usergroups_data = json.load(usergroups_f)
            print(zapi.hostgroup.create(usergroups_data))
#           for usergroup in usergroups_data:
#               # zapi.hostgroup.create(usergroup)
#               print(usergroup['name'])
        usergroups_f.close;

#         # Get all users (50)
# eh
#         user = json.dumps(zapi.user.get(output='extend'))
#         with open('data/imptest/user.json', 'r') as usergroups:
#             print(user, file=f)
#         f.close;

    except Exception as e:
        print("Something wierd happened: $?\n", file=sys.stderr);
        logging.error(traceback.format_exc())
        exit(2)

    zapi.user.logout()

except ZabbixAPIException as e:
    print("Some Zabbix API error: {}".format(e))

