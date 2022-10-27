#!/usr/bin/env python3
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker

from pyzabbix.api import ZabbixAPI, ZabbixAPIException
import json
import sys

try:
    from local_passwords import api_user, api_password, api_url
except:
    print("File local_passwords.py does not exists. Please refer to the README file", file=sys.stderr)
    print("and create the named file before running this again.", file=sys.stderr)
    exit(1)

'''
inventory_mode:
    -1: disabled
     0: manual
     1: automatic

funker dette?
     result2 = zapi.do_request('host.get',
                          {
                              'filter': {'status': 1},
                              'output': 'extend'
                          })

{
    "jsonrpc": "2.0",
    "method": "item.update",
    "params": {
        "itemid": "10092",
        "status": 0
    },
    "auth": "0424bd59b807674191e7d77572075f33",
    "id": 5
}
'''

verbose=0

try:
    # Create ZabbixAPI class instance
    zapi = ZabbixAPI(url=api_url, user=api_user, password=api_password)

    # Get all monitored hosts
    allhosts = zapi.host.get(output='extend', selectInventory='extend')

    print(json.dumps(allhosts,indent=4))

    sys.exit()

    for host in allhosts:
        if 'os' in host['inventory']:
            if host['inventory']['os'] == '':
                print("{} has no OS in its inventory".format(host['host']))
            else:
                print("{} has OS {}".format(host['host'], host['inventory']['os']))
        if 0 and host['host'] == "roysk.oslomet.no":
            print(json.dumps(host,indent=4))
#           hlookup=zapi.host.get(output=['hostid'], selectInventory='extend', hostids=host['hostid'])
            #print("URL A {}".format(hlookup['inventory']['url_a']))
            #print(json.dumps(hlookup[0]['inventory']['url_a'],indent=4))
            print(json.dumps(hlookup[0]['inventory'],indent=4))
#           #host_update_json = {
#           #    "hostid": host['hostid'],
#           #    "inventory_mode": 1
#           #};
#           #print(json.dumps(host_update_json,indent=4))
#           #result2 = zapi.do_request('host.update', host_update_json)
#           #result2 = zapi.host.print(json.dumps(host,indent=4))

    zapi.user.logout()
except ZabbixAPIException as e:
    print("Some Zabbix API error: {}".format(d))

