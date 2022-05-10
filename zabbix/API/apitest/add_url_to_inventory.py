#!/usr/bin/env python3
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker

from pyzabbix.api import ZabbixAPI
import json
from local_passwords import api_user, api_password, api_url

# Create ZabbixAPI class instance
zapi = ZabbixAPI(url=api_url, user=api_user, password=api_password)

# Get all monitored hosts
result1 = zapi.host.get(output='extend')

hostnames1 = [host['host'] for host in result1]
for host in result1:
    if host['inventory_mode'] == "-1":
        host_update_json = {
            "hostid": host['hostid'],
            "inventory_mode": 1
        };
        print(json.dumps(host_update_json,indent=4))
        result2 = zapi.do_request('host.update', host_update_json)

    if host['host'] == 'roysk.oslomet.no':
        host_update_json = {
            "hostid": host['hostid'],
            "inventory": {
                "url_a": "https://confluence.oslomet.no/display/SYSADM/{}".format(host['host']),
            },
        };
        print(json.dumps(host_update_json,indent=4))
        result2 = zapi.do_request('host.update', host_update_json)

zapi.user.logout()
