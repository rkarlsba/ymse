#!/opt/python_venv/bin/python3
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker

import argparse
import io
import json
import pandas as pd
from pyzabbix.api import ZabbixAPI, ZabbixAPIException
import sys

# Globals
verbose=0
builtinhelp=1
hostgroup_list=None
zabbix_host_base_url='https://zabbix.oslomet.no/zabbix/zabbix.php?action=host.edit&hostid='
firstline=0
global_debug=0
csv_separator='\t'

def die(s: str, exitcode: int = 1) -> None:
    print(s, file=sys.stderr)
    sys.exit(exitcode)

def debprint(s: str, debuglevel: int = 1) -> None:
    if debuglevel < global_debug:
        return
    print(f"\033[3m{s}\033[0m")

try:
    from local_passwords import api_user, api_password, api_url

except:
    die("File local_passwords.py does not exists. Please refer to the README file\nand create the named file before running this again.")

if __name__ == "__main__":
    # Argparse
    argparser = argparse.ArgumentParser(add_help=builtinhelp)

    argparser.add_argument("-H", "--hostgroup", type=str, help="List members of given hostgroup")
    argparser.add_argument("-O", "--hostgrouplist", action='store_true', help="List all hostgroups")
    argparser.add_argument("-o", "--hostlist", action='store_true', help="List all hosts")
    argparser.add_argument("-L", "--html", action='store_true', help="Output HTML (not implemented)")
    argparser.add_argument("-C", "--csv", action='store_false', help="Output CSV (default)")
    argparser.add_argument("-s", "--separator", type=str, help="CSV separator (default is tab, as in \\t) ir just 0x09 - currently hardcoded as tab")
    argparser.add_argument("-v", "--verbose", action='count', default=0, help="Be verbose, tell the user what's going on and what's not going on, what Trump had for breakfast and how many hours it's left to armageddon and don't save any time whatsoever")
    argparser.add_argument("-d", "--debug", action='store_true', help="Enable debugging")
    argparser.add_argument("-S", "--server", type=str, help=f"Zabbix Server URL, for instance https://my.zabbixsrv.tld/zabbix/api_jsonrpc.php (not implemented)")
    argparser.add_argument("-U", "--user", type=str, help="Username (not implemented)")
    argparser.add_argument("-P", "--password", type=str, help="Passsword (not implemented)")

    if not builtinhelp:
        argparser.add_argument("-h", "--help", action="store_true", help="Show this help message and exit")

    args = argparser.parse_args()

    if args.debug:
        global_debug+=1

    if args.html and args.csv:
        die("Doh! Can't output both HTML and CSV at the same time")

    if args.hostgroup is None and not args.hostgrouplist and not args.hostlist:
        die("[1] We need either --hostgroup <hostgroup>, --hostgrouplist or --hostlist")

    if args.hostgroup is not None and (args.hostgrouplist or args.hostlist):
        die("[2] We need either --hostgroup <hostgroup>, --hostgrouplist or --hostlist")

    if args.hostgrouplist and args.hostlist:
        die("A wee glitch in the matrix - please use either hostgrouplist or hostlist")

    elif args.html and args.csv:
        die("Can't output HTML and CSV at the same time")

    if args.csv:
        print("CSV, jo, digg!")

# Gammelt {{{
        
#   if args.hostgroup is None and not args.hostgrouplist:
#       die("We need either --hostgrouplist or --hostgroup <somegroup> to do something useful")

#   if args.hostgroup is None and not (args.hostgrouplist or args.hostlist):
#       die("We need either --hostgrouplist, --hostlist or --hostgroup <somegroup> to do something useful, not two of tme!")

#   if (args.hostgrouplist or args.hostlist) and (args.html or args.csv):
#       die("HTML and CSV output are not supported for the hostgroup or host lists")
# }}}

    # Main code
    try:
        # Create ZabbixAPI class instance
        zapi = ZabbixAPI(server=api_url)

        # Login to Zabbix
        zapi.login(user=api_user, password=api_password)

        if args.hostgrouplist:
            hostgroup_filter = { }
        else:
            hostgroup_filter = { "name": args.hostgroup }
        hostgroup_list = zapi.hostgroup.get(filter=hostgroup_filter, output=['hostid', 'name', 'status'], selectHosts=['hostid', 'host', 'status'])

        host_filter = { }
        host_list = zapi.host.get(filter=host_filter, output=['hostid', 'host', 'status'], selectHosts=['hostid', 'host', 'status'], selectGroups='extend')

        if (args.hostlist):
            for h in sorted(host_list, key=lambda d: d["host"].lower()):
                print(h["host"])
            sys.exit(0)

        if args.hostgrouplist:
            for hg in hostgroup_list:
                print(hg["name"])
            sys.exit(0)

        count=0
        for host in sorted(hostgroup_list[0]["hosts"], key=lambda d: d["host"].lower()):
            status=''
            count += 1
            if host["status"] == "1":
                status='\tDISABLED'
            if args.csv or args.html:
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
                if count == len(hostgroup_list[0]["hosts"]):
                    print(f"\nFound a total of {count} hosts in hostgroup {args.hostgroup}")

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

