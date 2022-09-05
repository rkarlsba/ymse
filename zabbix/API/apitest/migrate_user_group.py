#!/usr/bin/env python3
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker

from pyzabbix.api import ZabbixAPI, ZabbixAPIException
import json
import sys
from pprint import pprint

try:
    from local_passwords import *
except:
    print("File local_passwords.py does not exists. Please refer to the README.md file", file=sys.stderr)
    print("and create the named file before running this again.", file=sys.stderr)
    exit(1)

verbose = 0

try:
    # Create ZabbixAPI class instance
    zapi = ZabbixAPI(url=api_url, user=api_user, password=api_password)

    # Get all users
    allusers = json.dumps(zapi.user.get(output='extend'))
    print('allusers is of type ', type(allusers))
    print(allusers)

    allusers_j = json.loads(allusers)
    pprint(allusers_j)

    zapi.user.logout()
except ZabbixAPIException as e:
    print("Some Zabbix API error: {}".format(d))

