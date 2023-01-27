#!/usr/bin/env python3
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker
import json
import json
import sys
from pprint import pprint
import traceback
import logging
import random
import string
all = string.ascii_letters + string.digits + string.punctuation

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
    # zapi = ZabbixAPI(url=api_url, user=api_user, password=api_password)
    zapi_dst = ZabbixAPI(url=api_dst_url, user=api_dst_user, password=api_dst_password)
    print(zapi_dst.api_version())


    # Get all users (50)
    # user = json.dumps(zapi.user.get(output='extend', selectUsrgrps="extend", selectRole="extend", selectMedias="extend", selectMediatypes="extend"), indent=4)
    with open(f'{outputdir}/user.load.json', 'r') as f:
        users = json.load(f)

    # Get all usergroups (52)
    # usergroup = json.dumps(zapi.usergroup.get(output='extend', selectTagFilters='extend', selectUsers='extend', selectHostGroupRights='extend', selectTemplateGroupRights='extend'), indent=4)
    with open(f'{outputdir}usergroup.load.json', 'r') as f:
        usergroup = json.load(f)

    grupper = []
    for grp in usergroup:
        grupper = zapi_dst.usergroup.get()
        if grp.get("name") not in [n.get("name") for n in grupper]:
            print(f"Lag gruppe {grp.get('name')}")
            newgrp = zapi_dst.usergroup.create(name=grp.get('name'),
                                               gui_access=grp.get('gui_access'), debug_mode=grp.get('debug_mode'))
            print(newgrp)

    remove_param = ['userid', 'attempt_failed', 'attempt_ip', 'attempt_clock', 'mediatypes']

    for usr in users:
        brukere = zapi_dst.user.get()
        if usr.get("alias") not in [n.get("username") for n in brukere]:
            print(f"Lag user {usr.get('alias')}")
            password = "".join(random.sample(all, 20))  # lag random passord
            print(f'user: {usr.get("alias")} pass: {password}')  # vis det..
            usr["roleid"] = usr.pop("type")  # type ble til roleid i versjon 5.2 (?)
            usrgrps = usr.pop("usrgrps")
            medias = usr.pop("medias")
            usr["usrgrps"] = [{'usrgrpid': g.get("usrgrpid")} for gr in usrgrps for g in grupper if g.get("name") == gr.get("name")]
            print(usr["usrgrps"])

            for a in remove_param:
                usr.pop(a)
            newUser = zapi_dst.user.create(passwd=password, **usr)
            print(newUser)

except Exception as e:
    print(f"Exception: {e}")