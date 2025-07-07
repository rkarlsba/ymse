#!/usr/bin/env python3

"""
Se https://dl.dell.com/manuals/all-products/esuprt_software/esuprt_ent_sys_mgmt/openmanage-essentials-v25_white-papers_en-us.pdf for detailjer om API-et
"""

import requests
import json
import sys
import argparse
import urllib3
import logging
import time

urllib3.disable_warnings(urllib3.exceptions.InsecureRequestWarning)

# Try to import defaults from zabbix_auth.py, but don't crash if missing
try:
    from zabbix_auth import ome_host as default_ome_host, ome_user as default_ome_user, ome_pass as default_ome_pass
except ImportError:
    default_ome_host = None
    default_ome_user = None
    default_ome_pass = None

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

def timed_get(label, url, headers, verbose):
    start = time.time()
    logging.info(f"Requesting {label}: {url}")
    try:
        r = requests.get(url, headers=headers, verify=False)
    except Exception as e:
        logging.error(f"Error during {label} request: {e}")
        raise
    elapsed = time.time() - start
    logging.info(f"{label} took {elapsed:.2f} seconds, status {r.status_code}")
    if verbose >= 2:
        logging.debug(f"Response: {r.text[:1000]}")  # Show up to 1000 chars
    return r

def get_auth_token(ome_host, ome_user, ome_pass, verbose):
    url = f"{ome_host}/api/SessionService/Sessions"
    payload = {
        "UserName": ome_user,
        "Password": ome_pass,
        "SessionType": "API"
    }
    headers = {"Content-Type": "application/json"}
    start = time.time()
    logging.info(f"Authenticating to {ome_host}")
    r = requests.post(url, json=payload, headers=headers, verify=False)
    elapsed = time.time() - start
    logging.info(f"Auth took {elapsed:.2f} seconds, status {r.status_code}")
    if verbose >= 2:
        logging.debug(f"Auth response: {r.text}")
    r.raise_for_status()
    return r.headers["X-Auth-Token"]

def get_devices(ome_host, auth_token, verbose):
    url = f"{ome_host}/api/DeviceService/Devices"
    headers = {"X-Auth-Token": auth_token}
    r = timed_get("Devices", url, headers, verbose)
    r.raise_for_status()
    return r.json().get("value", [])

def find_device_id_by_name(ome_host, ome_user, ome_pass, device_name, verbose):
    token = get_auth_token(ome_host, ome_user, ome_pass, verbose)
    devices = get_devices(ome_host, token, verbose)
    for device in devices:
        if device.get("DeviceName") == device_name:
            return device.get("Id")
    return None

def get_full_device_info(ome_host, auth_token, device_id, verbose):
    headers = {"X-Auth-Token": auth_token}
    def get(label, endpoint):
        url = f"{ome_host}{endpoint}"
        r = timed_get(label, url, headers, verbose)
        if r.status_code == 200:
            try:
                return r.json()
            except Exception as e:
                logging.error(f"Error decoding JSON for {label}: {e}")
                return {"error": f"JSON decode error for {endpoint}"}
        else:
            return {"error": f"HTTP {r.status_code} for {endpoint}"}
    return {
        "device": get("Device", f"/api/DeviceService/Devices({device_id})"),
        "inventory": get("Inventory", f"/api/DeviceService/Devices({device_id})/InventoryDetails"),
        "firmware": get("Firmware", f"/api/DeviceService/Devices({device_id})/FirmwareInventory"),
        "subsystem_health": get("SubsystemHealth", f"/api/DeviceService/Devices({device_id})/SubSystemHealth"),
        "hardware_logs": get("HardwareLogs", f"/api/DeviceService/Devices({device_id})/HardwareLogs"),
        "recent_activity": get("RecentActivity", f"/api/DeviceService/Devices({device_id})/RecentActivity"),
        "warranty": get("Warranty", f"/api/WarrantyService/Warranties?DeviceId={device_id}"),
        "licenses": get("Licenses", f"/api/LicenseService/Devices({device_id})/Licenses"),
        "alerts": get("Alerts", f"/api/AlertService/Alerts?$filter=DeviceId eq {device_id}"),
        "compliance": get("Compliance", f"/api/ComplianceService/Devices({device_id})/ComplianceReports"),
        "power_metrics": get("PowerMetrics", f"/api/PowerManagerService/Devices({device_id})/Metrics"),
        "power_cap": get("PowerCap", f"/api/PowerManagerService/Devices({device_id})/PowerCap"),
        "supportassist_client": get("SupportAssistClient", f"/api/SupportAssistService/Registration/ClientDetails"),
        "supportassist_cases": get("SupportAssistCases", f"/api/SupportAssistService/Cases?$filter=DeviceId eq {device_id}"),
        "network": get("Network", f"/api/DeviceService/Devices({device_id})/NetworkInterfaces"),
        "groups": get("Groups", f"/api/GroupService/Devices({device_id})/Groups"),
    }

def do_discover(ome_host, ome_user, ome_pass, verbose):
    try:
        token = get_auth_token(ome_host, ome_user, ome_pass, verbose)
        devices = get_devices(ome_host, token, verbose)
        lld_data = []
        for device in devices:
            lld_data.append({
                "{#DEVICEID}": device.get("Id"),
                "{#DEVICENAME}": device.get("DeviceName"),
                "{#MODEL}": device.get("Model"),
                "{#DEVICESERVICETAG}": device.get("DeviceServiceTag"),
                "{#CHASSISSERVICETAG}": device.get("ChassisServiceTag"),
                "{#IPADDRESS}": device.get("IpAddress"),
                "{#ASSETTAG}": device.get("AssetTag"),
                "{#SERIALNUMBER}": device.get("SerialNumber"),
                "{#SYSTEMID}": device.get("SystemId"),
                "{#TYPE}": device.get("Type"),
                "{#STATUS}": device.get("Status"),
                "{#HEALTH}": device.get("Health"),
                "{#LASTINVENTORYTIME}": device.get("LastInventoryTime"),
                "{#LASTSTATUSTIME}": device.get("LastStatusTime"),
            })
        print(json.dumps({"data": lld_data}, indent=2))
    except Exception as e:
        logging.error(str(e))
        print(json.dumps({"error": str(e)}))

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Dell OME Zabbix Discovery and Full Device Info Script")
    parser.add_argument('--ome-host', help='OME server URL (e.g. https://ome.example.com)', default=default_ome_host)
    parser.add_argument('--ome-user', help='OME username', default=default_ome_user)
    parser.add_argument('--ome-pass', help='OME password', default=default_ome_pass)
    parser.add_argument('-v', '--verbose', action='count', default=0, help='Increase output verbosity (repeat for more)')
    group = parser.add_mutually_exclusive_group(required=True)
    group.add_argument('--discover', action='store_true', help='Run in discovery mode (LLD)')
    group.add_argument('--device-id', metavar='DEVICE_ID', help='Get all info for DEVICE_ID')
    group.add_argument('--device-name', metavar='DEVICE_NAME', help='Get all info for DEVICE_NAME')
    args = parser.parse_args()

    setup_logging(args.verbose)

    # Ensure all credentials are present
    if not args.ome_host or not args.ome_user or not args.ome_pass:
        print(json.dumps({"error": "OME host, username, and password must be provided (via arguments or zabbix_auth.py)"}))
        sys.exit(1)

    if args.discover:
        do_discover(args.ome_host, args.ome_user, args.ome_pass, args.verbose)
    else:
        try:
            token = get_auth_token(args.ome_host, args.ome_user, args.ome_pass, args.verbose)
            if args.device_id:
                device_id = args.device_id
            else:
                device_id = find_device_id_by_name(args.ome_host, args.ome_user, args.ome_pass, args.device_name, args.verbose)
                if not device_id:
                    print(json.dumps({"error": f"Device with name '{args.device_name}' not found"}))
                    sys.exit(1)
            all_info = get_full_device_info(args.ome_host, token, device_id, args.verbose)
            print(json.dumps(all_info, indent=2))
        except Exception as e:
            logging.error(str(e))
            print(json.dumps({"error": str(e)}))

