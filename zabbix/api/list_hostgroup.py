#!/usr/bin/env python3
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker

import argparse
import json
from pyzabbix.api import ZabbixAPI, ZabbixAPIException
import sys

try:
    from local_passwords import api_user, api_password, api_url

except:
    print("File local_passwords.py does not exists. Please refer to the README file", file=sys.stderr)
    print("and create the named file before running this again.", file=sys.stderr)
    exit(1)

# Globals
verbose=0
hostgroupname=''
builtinhelp=1
hostgroup=None

if __name__ == "__main__":
    # <argparse>

    argparser = argparse.ArgumentParser(add_help=builtinhelp)

    argparser.add_argument("-H", "--hostgroup", type=str, help="List members of given hostgroup")
    argparser.add_argument("-S", "--server", type=str, help=f"Zabbix Server URL, for instance https://my.zabbixsrv.tld/zabbix/api_jsonrpc.php")
    argparser.add_argument("-U", "--user", type=str, help="Username")
    argparser.add_argument("-P", "--password", type=str, help="Passsword")
    # argparser.add_argument("-F", "--password-file", type=str, help="Path to passsword file with user:s3Cr3t")

    if not builtinhelp:
        argparser.add_argument("-h", "--help", action="store_true", help="Show this help message and exit")

    args = argparser.parse_args()

    if args.hostgroup == None:
        print("We need a hostgroup");
        exit(1)

    hostgroup = args.hostgroup

    # </argparse>

    try:
        # Create ZabbixAPI class instance
        zapi = ZabbixAPI(server=api_url)

        # Login to Zabbix
        zapi.login(user=api_user, password=api_password)

        # json_filter = '{"name": "' + f"{args.hostgroup}" + '"}'
        json_filter = { "name": args.hostgroup }
        # print(json.dumps(json_filter,indent=4))
        #hostgroup = zapi.hostgroup.get()
        #print(f"Denne: '{args.hostgroup}'")
        #print(json.dumps(json_filter,indent=4))
        # hostgroup = zapi.hostgroup.get(filter={"name": args.hostgroup}, output='extend')
        # hostgroup = zapi.hostgroup.get(filter={"name": "Maintenance"}, output='extend')
        hostgroup = zapi.hostgroup.get(filter=json_filter, output=['hostid', 'name'], selectHosts=['hostid', 'host'])
        # print(f"{args.hostgroup}\nMaintenance")

        # hostgroup = zapi.hostgroup.get(filter={"name": args.hostgroup}, output=['hostid', 'name'], selectHosts=['hostid', 'host'])
        # hostgroup = zapi.hostgroup.get(filter=json_filter, output=['hostid', 'name'], selectHosts=['hostid', 'host'])
        # hostgroup = zapi.hostgroup.get(output=['hostid', 'name'], selectHosts=['hostid', 'host'])

        # Print the object
        # hostgroup_id = hostgroup[0].groupid
        # hostgroup_txt = json.dumps(hostgroup,indent=4);
        # print(f"{hostgroup_txt}")
        # print("{hostgroupid}")
        print(json.dumps(hostgroup,indent=4))

#       for host in allhosts:
#           if 'os' in host['inventory']:
#               if host['inventory']['os'] == '':
#                   print("{} has no OS in its inventory".format(host['host']))
#               else:
#                   print("{} has OS {}".format(host['host'], host['inventory']['os']))
#           if 0 and host['host'] == "roysk.oslomet.no":
#               print(json.dumps(host,indent=4))
#               print(json.dumps(hlookup[0]['inventory'],indent=4))

        zapi.user.logout()
    except ZabbixAPIException as e:
        print("Zabbix API error: {}".format(d))
        exit(1)

