#!/usr/bin/env python3
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker

import argparse
import requests
import logging
import sys
import json

from zabbix_auth import *

# Disable SSL warnings (optional)
requests.packages.urllib3.disable_warnings()

devices=[
    10200, 10201, 10203, 10204, 10206, 10211, 10214, 10216, 10217, 10218,
    10219, 10220, 10221, 10222, 10223, 10224, 10225, 10226, 13916, 13917,
    45723, 45724, 64765, 64877, 67872, 73156, 84482, 89251, 90207,
]

endpoints=[
    '/api/DeviceService/Devices(64877)/FirmwareInventory',
    '/api/DeviceService/Devices(64877)/HardwareLogs',
    '/api/LicenseService/Devices(64877)/Licenses',
    '/api/AlertService/Alerts?$filter=DeviceId,
    '/api/ComplianceService/Devices(64877)/ComplianceReports',
    '/api/PowerManagerService/Devices(64877)/Metrics',
    '/api/PowerManagerService/Devices(64877)/PowerCap',
    '/api/SupportAssistService/Registration/ClientDetails',
    '/api/SupportAssistService/Cases?$filter=DeviceId,
    '/api/DeviceService/Devices(64877)/NetworkInterfaces',
    '/api/GroupService/Devices(64877)/Groups',
]

def setup_logging(verbosity):
    if verbosity == 0:
        level = logging.WARNING
    elif verbosity == 1:
        level = logging.INFO
    else:
        level = logging.DEBUG
    logging.basicConfig(format="%(asctime)s %(levelname)s: %(message)s", level=level)
    if verbosity >= 3:
        try:
            import http.client as http_client
        except ImportError:
            import httplib as http_client
        http_client.HTTPConnection.debuglevel = 1
        logging.getLogger("urllib3").setLevel(logging.DEBUG)
        logging.getLogger("urllib3").propagate = True
        logging.debug("HTTP debug level enabled")

def main():
    parser = argparse.ArgumentParser(description="Query Dell OME API endpoint and print JSON response")
    parser.add_argument('-v', '--verbose', action='count', default=0, help='Increase output verbosity (repeat for more)')
    parser.add_argument('endpoint', metavar='endpoint', help='OME API endpoint to query (e.g., /api/AlertService/Alerts)')
    args = parser.parse_args()

    setup_logging(args.verbose)

    # Step 1: Authenticate
    session_url = f"{ome_host}/api/SessionService/Sessions"
    session_data = {
        "UserName": ome_user,
        "Password": ome_pass
    }

    logging.debug(f"Authenticating to {session_url}")
    session = requests.post(session_url, json=session_data, verify=False)
    if session.status_code != 201:
        logging.error(f"Authentication failed: {session.status_code}")
        sys.exit(1)

    cookies = session.cookies

    # Step 2: Query the specified endpoint
    full_url = f"{ome_host}{args.endpoint}"
    logging.debug(f"Querying endpoint: {full_url}")
    response = requests.get(full_url, cookies=cookies, verify=False)

    if response.status_code == 200:
        print(json.dumps(response.json(), indent=2))
    else:
        logging.error(f"Failed to fetch data: {response.status_code}")
        sys.exit(1)

if __name__ == "__main__":
    main()

