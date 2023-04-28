#!/usr/bin/env python3
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker

from pyzabbix.api import ZabbixAPI, ZabbixAPIException
import json
import sys
from pprint import pprint

# Globals
debug = 0
maxhosts = -1
cleanup = 0

def debprint(s):
    if debug:
        print(f"[DEBUG] {s}")

# Read local_passwords.py
try:
    from local_passwords import *
except:
    print("File local_passwords.py does not exists. Please refer to the README file", file=sys.stderr)
    print("and create the named file before running this again.", file=sys.stderr)
    exit(1)
##

missing_hosts_fn = "mangler.txt"
debprint("# vim:ft=json")

try:
    missing_hosts_fd = open(missing_hosts_fn, "r")
    missing_hosts_data = missing_hosts_fd.read()
    missing_hosts_fd.close()
except IOError:
    print(f"Could not read file: {missing_hosts_fn}")

missing_hosts = missing_hosts_data.replace('\n', ' ').split(' ')
missing_hosts.remove('')

try:
    # Create ZabbixAPI class instance
    zapi_src = ZabbixAPI(url=api_src_url, user=api_src_user, password=api_src_password)
    hostcount=0

    # Get all monitored hosts
    allhosts = zapi_src.host.get(output='extend', selectInventory='extend')
    debprint(f"allhosts is of type {type(allhosts)}")
    for host in allhosts:
        debprint(f"host is of type {type(host)}")
        if host['host'] in missing_hosts:
            debprint(f" ===== {host['host']} =====")

            hostinterfaces = zapi_src.hostinterface.get(output='extend', hostids=host['hostid'])
            debprint(f"hostinterfaces is of type {type(hostinterfaces)}") # list
            if cleanup:
                del host["hostid"];
            print(json.dumps(host, indent=4))
            for hostint in hostinterfaces:
                debprint(f"hostint is of type {type(hostint)}") # dict
                if cleanup:
                    del hostint["interfaceid"];
                    del hostint["hostid"];
                print(json.dumps(hostint, indent=4))

# og så er det bare å kjøre zapi_dst.host.create
# https://www.zabbix.com/documentation/current/en/manual/api/reference/host/create

            hostcount+=1
            if (maxhosts > 0 and hostcount >= maxhosts):
                sys.exit()


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

