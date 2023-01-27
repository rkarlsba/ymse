#!/usr/bin/env python3
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker
import json
import json
import sys
from pprint import pprint
import traceback
import logging

try:
    from pyzabbix.api import ZabbixAPI, ZabbixAPIException
except:
    print("You'll need ZabbixAPI and its friends to make any use of these tools", file=sys.stderr)
    exit(1)

try:
    from local_passwords import *
except:
    print("File local_passwords.py does not exists. Please refer to the README.md file", file=sys.stderr)
    print("and create the named file before running this again.", file=sys.stderr)
    exit(1)

# Globals
verbose = 0
outputdir = "data/"

if verbose:
    stream = logging.StreamHandler(sys.stdout)
    stream.setLevel(logging.DEBUG)
    log = logging.getLogger('pyzabbix')
    log.addHandler(stream)
    log.setLevel(logging.DEBUG)

try:
    # Create ZabbixAPI class instance
    zapi = ZabbixAPI(url=api_url, user=api_user, password=api_password)
    zapi_dst = ZabbixAPI(url=api_dst_url, user=api_dst_user, password=api_dst_password)
    print(zapi.api_version())


    # Get all users (50)
    user = json.dumps(zapi.user.get(output='extend', selectUsrgrps="extend", selectRole="extend", selectMedias="extend", selectMediatypes="extend"), indent=4)
    with open(f'{outputdir}/user.load.json', 'w') as f:
        print(user, file=f)


    # Get all usergroups (52)
    usergroup = json.dumps(zapi.usergroup.get(output='extend', selectTagFilters='extend', selectUsers='extend', selectHostGroupRights='extend', selectTemplateGroupRights='extend'), indent=4)
    with open(f'{outputdir}usergroup.load.json', 'w') as f:
        print(usergroup, file=f)


except Exception as e:
    print(f"Exception: {e}")