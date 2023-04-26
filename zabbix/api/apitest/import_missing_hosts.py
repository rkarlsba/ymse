#!/usr/bin/env python3
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker

from pyzabbix.api import ZabbixAPI, ZabbixAPIException
import json
import sys
from pprint import pprint

# Read local_passwords.py
try:
    from local_passwords import *
except:
    print("File local_passwords.py does not exists. Please refer to the README file", file=sys.stderr)
    print("and create the named file before running this again.", file=sys.stderr)
    exit(1)
##

missing_hosts_fn = "mangler.txt"

try:
    missing_hosts_fd = open(missing_hosts_fn, "r")
    missing_hosts_data = missing_hosts_fd.read()
    missing_hosts_fd.close()
except IOError:
    print(f"Could not read file: {missing_hosts_fn}")

missing_hosts = missing_hosts_data.replace('\n', ' ').split(' ')
missing_hosts.remove('')
# print(f"Missing hosts [{len(missing_hosts)}]: {missing_hosts}")
# sys.exit()

try:
    # Create ZabbixAPI class instance
    zapi_src = ZabbixAPI(url=api_src_url, user=api_src_user, password=api_src_password)

    # Get all monitored hosts
    allhosts = zapi_src.host.get(output='extend', selectInventory='extend')
    for host in allhosts:
        if host['host'] in missing_hosts:
            print(host['host'])

    # print(json.dumps(allhosts,indent=4))
#         if 0 and host['host'] == "roysk.oslomet.no":
#             print(json.dumps(host,indent=4))
# #           hlookup=zapi.host.get(output=['hostid'], selectInventory='extend', hostids=host['hostid'])
#             #print("URL A {}".format(hlookup['inventory']['url_a']))
#             #print(json.dumps(hlookup[0]['inventory']['url_a'],indent=4))
#             print(json.dumps(hlookup[0]['inventory'],indent=4))
# #           #host_update_json = {
# #           #    "hostid": host['hostid'],
# #           #    "inventory_mode": 1
# #           #};
# #           #print(json.dumps(host_update_json,indent=4))
# #           #result2 = zapi.do_request('host.update', host_update_json)
# #           #result2 = zapi.host.print(json.dumps(host,indent=4))

    zapi_src.user.logout()
except ZabbixAPIException as e:
    print("Some Zabbix API error: {}".format(d))

