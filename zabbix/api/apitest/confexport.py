#!/usr/bin/env python3
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker

import json
import sys
from pprint import pprint
import traceback
import logging
import datetime

now = datetime.datetime.now()

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
outputdir = "confexport/prod/"  # need tailing slash

debug = True
if debug:
    stream = logging.StreamHandler(sys.stdout)
    stream.setLevel(logging.DEBUG)
    log = logging.getLogger('pyzabbix')
    log.addHandler(stream)
    log.setLevel(logging.DEBUG)

try:
    # Create ZabbixAPI class instance
    zapi = ZabbixAPI(url=api_url, user=api_user, password=api_password)  # 5.0.28
    # zapi = ZabbixAPI(url=api_dst_url, user=api_dst_user, password=api_dst_password)  # 6.0.9
    # print(zapi.api_version())

    exp = {}  # hva som skal eksporteres
    exp["groups"] = [i.get("groupid") for i in zapi.hostgroup.get()]
    exp["hosts"] = [i.get("hostid") for i in zapi.host.get()]
    exp["images"] = [i.get("imageid") for i in zapi.image.get()]
    exp["maps"] = [i.get("sysmapid") for i in zapi.map.get()]
    exp["mediaTypes"] = [i.get("mediatypeid") for i in zapi.mediaType.get()]

    # ikke i zabbix v6.0:
    # exp["screens"] = [i.get("screenid") for i in zapi.screen.get()]
    # exp["valueMaps"] = [i.get("valuemapid") for i in zapi.valueMap.get()]

    # https://www.zabbix.com/documentation/6.0/en/manual/api/reference/configuration/export
    for exportnavn in exp:
        obj = zapi.configuration.export(format="json", options={exportnavn: exp.get(exportnavn)})
        exportfile = f"{now:%F}-{exportnavn}"
        with open(f'{outputdir}{exportfile}.json', 'w') as f:
             print(obj, file=f)
        f.close()

    # egen template exporter nedenfor skriver en fil pr template.
    # Ga server_code 500 ved export fordi den ble for stor.
    outputdir = f"{outputdir}templates/"  # need tailing slash
    for t in zapi.template.get():
        templateid = t.get("templateid")
        obj = zapi.configuration.export(format="json", options={"templates": [templateid]})

        # exportfile = f"{now:%F}." + "-".join(exp)
        exportfile = templateid
        with open(f'{outputdir}{exportfile}.json', 'w') as f:
             print(obj, file=f)
        f.close()

except Exception as e:
    print(e)


# https://www.zabbix.com/documentation/6.0/en/manual/api/reference/configuration/importcompare
# https://www.zabbix.com/documentation/6.0/en/manual/api/reference/configuration/import

# tipper import blir som dette:
# rules = {
#   "groups": {
#       "createMissing": True,
#       "updateExisting": True
#   },
# zapi.configuration.import(format="json", rules=rules, source=readfile(groups-fila))  # readfile trenger fikses..
#                                                                                      # dump inn hele fila der..
