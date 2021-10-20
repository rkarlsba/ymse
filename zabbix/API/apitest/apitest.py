#!/usr/bin/env python3
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker

from pyzabbix.api import ZabbixAPI
import json
from local_passwords import api_user, api_password, api_url

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

# Create ZabbixAPI class instance
zapi = ZabbixAPI(url=api_url, user=api_user, password=api_password)

# Get all monitored hosts
result1 = zapi.host.get(output='extend')

hostnames1 = [host['host'] for host in result1]
for host in result1:
    if host['inventory_mode'] == "-1":
        #print(json.dumps(host,indent=4))
        host_update_json = {
            "hostid": host['hostid'],
            "inventory_mode": 1
        };
        print(json.dumps(host_update_json,indent=4))
        result2 = zapi.do_request('host.update', host_update_json)
        #result2 = zapi.host.print(json.dumps(host,indent=4))

zapi.user.logout()
