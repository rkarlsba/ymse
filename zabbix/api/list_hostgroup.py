#!/opt/python_venv/bin/python3
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker

import argparse
import io
import json
import pandas as pd
from pyzabbix.api import ZabbixAPI, ZabbixAPIException
import sys

try:
    from local_passwords import api_user, api_password, api_url

except:
    print("File local_passwords.py does not exists. Please refer to the README file", file=sys.stderr)
    print("and create the named file before running this again.", file=sys.stderr)
    exit(1)

if __name__ == "__main__":
    # Globals
    verbose=0
    builtinhelp=1
    hostgroup=None
    zabbix_host_base_url='https://zabbix.oslomet.no/zabbix/zabbix.php?action=host.edit&hostid='
    firstline=0

    # Argparse
    argparser = argparse.ArgumentParser(add_help=builtinhelp)

    argparser.add_argument("-H", "--hostgroup", type=str, help="List members of given hostgroup")
    argparser.add_argument("-l", "--list", action='store_true', help="List all hostgroups")
    argparser.add_argument("-L", "--html", action='store_true', help="Output HTML")
    argparser.add_argument("-C", "--csv", action='store_true', help="Output CSV")
    argparser.add_argument("-v", "--verbose", action='store_true', help="Be verbose, tell the user what's going on and what's not going on, what Trump had for breakfast and how many hours it's left to armageddon and don't save any time whatsoever")
    argparser.add_argument("-S", "--server", type=str, help=f"Zabbix Server URL, for instance https://my.zabbixsrv.tld/zabbix/api_jsonrpc.php (not implemented)")
    argparser.add_argument("-U", "--user", type=str, help="Username (not implemented)")
    argparser.add_argument("-P", "--password", type=str, help="Passsword (not implemented)")

    if not builtinhelp:
        argparser.add_argument("-h", "--help", action="store_true", help="Show this help message and exit")

    args = argparser.parse_args()

    if args.html and args.csv:
        print("Doh! Can't output both HTML and CSV at the same time")
        sys.exit(1)
        
    if args.hostgroup is None and args.list is None:
        print("We need either --list or --hostgroup <somegroup> to do something useful")
        sys.exit(1)

    hostgroup = args.hostgroup

    # Main code
    try:
        # Create ZabbixAPI class instance
        zapi = ZabbixAPI(server=api_url)

        # Login to Zabbix
        zapi.login(user=api_user, password=api_password)

        hostgroup_filter = { "name": args.hostgroup }
        hostgroup = zapi.hostgroup.get(filter=hostgroup_filter, output=['hostid', 'name', 'status'], selectHosts=['hostid', 'host', 'status'])

        host_filter = { }
        all_hosts = zapi.host.get()

        #print(json.dumps(hostgroup,indent=4))
        #print(json.dumps(all_hosts,indent=4))

        count=0
        for host in sorted(hostgroup[0]["hosts"], key=lambda d: d["host"].lower()):
            status=''
            count += 1
            if host["status"] == "1":
                status='\tDISABLED'
            elif args.csv or args.html:
                if firstline == 0:
                    csv = "hostid,hostname,disabled\n"
                    firstline=1
                if args.html:
                    zabbix_host_url = zabbix_host_base_url+str(host["hostid"])
                    csvl = f'{host["hostid"]},<a href="{zabbix_host_url}">{host["host"]}</a>,{host["status"]}\n'
                    csv += csvl
                else:
                    csv += f'{host["hostid"]},{host["host"]},{host["status"]}\n'
            else:
                print(host["host"]+status)
                if count == len(hostgroup[0]["hosts"]):
                    print(f"\nFound a total of {count} hosts in hostgroup")

        if args.csv:
            print(csv)
        elif args.html:
            title = "Zabbix report for hostgroup"
            html = f"""<html>
    <head>
        <title>{title} {args.hostgroup}</title>
    </head>
    <body>
        <h1>{title} <b>{args.hostgroup}</b></h1>
        <hr width="60%">\n"""
            csvbuf = io.StringIO(csv)
            htmlobj = pd.read_csv(csvbuf)
            html += htmlobj.to_html(index=False, justify="left")
            html = html.replace('&lt;', '<')
            html = html.replace('&gt;', '>')
            print(html)

        zapi.user.logout()
    except ZabbixAPIException as e:
        print("Zabbix API error: {}".format(d))
        exit(1)

