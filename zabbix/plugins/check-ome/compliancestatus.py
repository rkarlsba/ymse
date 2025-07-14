#!/usr/bin/env python3
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker

import argparse
import requests
import logging
import sys
import json

from zabbix_auth import ome_host, ome_user, ome_pass

# Disable SSL warnings (optional)
requests.packages.urllib3.disable_warnings()

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

def authenticate():
    """Authenticate to OME and return X-Auth-Token header."""
    session_url = f"{ome_host}/api/SessionService/Sessions"
    session_data = {
        "UserName": ome_user,
        "Password": ome_pass
    }
    logging.debug(f"Authenticating to {session_url}")
    try:
        resp = requests.post(session_url, json=session_data, verify=False)
    except Exception as e:
        logging.error(f"Connection error: {e}")
        sys.exit(1)
    if resp.status_code != 201:
        logging.error(f"Authentication failed: {resp.status_code} {resp.text}")
        sys.exit(1)
    x_auth_token = resp.headers.get('X-Auth-Token')
    if not x_auth_token:
        logging.error("Failed to retrieve X-Auth-Token from authentication response.")
        sys.exit(1)
    return {'X-Auth-Token': x_auth_token}

def main():
    parser = argparse.ArgumentParser(description="Query Dell OME API endpoint and print JSON response")
    parser.add_argument('-v', '--verbose', action='count', default=0, help='Increase output verbosity (repeat for more)')
    # parser.add_argument('endpoint', metavar='endpoint', help='OME API endpoint to query (e.g., /api/AlertService/Alerts)')
    args = parser.parse_args()

    setup_logging(args.verbose)

    headers = authenticate()

    # Step 2: Query the specified endpoint
    #full_url = f"{ome_host}{args.endpoint}"
    Id=8
    full_url = f"{ome_host}/api/UpdateService/ComplianceStatuses?$filter=DeviceId eq {Id}"
    logging.debug(f"Querying endpoint: {full_url}")
    try:
        response = requests.get(full_url, headers=headers, verify=False)
    except Exception as e:
        logging.error(f"Connection error: {e}")
        sys.exit(1)

    if response.status_code == 200:
        try:
            print(json.dumps(response.json(), indent=2))
        except Exception as e:
            logging.error(f"Failed to parse JSON response: {e}")
            print(response.text)
    else:
        logging.error(f"Failed to fetch  {response.status_code} {response.text}")
        sys.exit(1)

if __name__ == "__main__":
    main()

