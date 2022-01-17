#!/usr/bin/env python3
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker

from pyzabbix.api import ZabbixAPI
import json
from local_passwords import api_user, api_password, api_url

# Create ZabbixAPI class instance
zapi = ZabbixAPI(url=api_url, user=api_user, password=api_password)

# Get all monitored hosts
result1 = zapi.host.get(hostid=10194, output='extend')

hostnames1 = [host['host'] for host in result1]
#hostnames1 = [inventory_mode['host'] for host in result1]

#print(json.dumps(result1, indent=4))
#print(result1)
#print(hostnames1)
for host in result1:
    print('{} {}'.format(host['host'], host['inventory_mode']))

zapi.user.logout()
