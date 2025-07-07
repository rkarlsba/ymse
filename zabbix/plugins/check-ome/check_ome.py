#!/usr/bin/env python3

"""
Se https://dl.dell.com/manuals/all-products/esuprt_software/esuprt_ent_sys_mgmt/openmanage-essentials-v25_white-papers_en-us.pdf for detailjer om API-et
"""

import requests
import json
import sys
import argparse
import urllib3

urllib3.disable_warnings(urllib3.exceptions.InsecureRequestWarning)

# Try to import defaults from zabbix_auth.py, but don't crash if missing
try:
    from zabbix_auth import ome_host as default_ome_host, ome_user as default_ome_user, ome_pass as default_ome_pass
except ImportError:
    default_ome_host = None
    default_ome_user = None
    default_ome_pass = None

def get_auth_token(ome_host, ome_user, ome_pass):
    url = f"{ome_host}/api/SessionService/Sessions"
    payload = {
        "UserName": ome_user,
        "Password": ome_pass,
        "SessionType": "API"
    }
    headers = {"Content-Type": "application/json"}
    response = requests.post(url, json=payload, headers=headers, verify=False)
    response.raise_for_status()
    return response.headers["X-Auth-Token"]

def get_devices(ome_host, auth_token):
    url = f"{ome_host}/api/DeviceService/Devices"
    headers = {"X-Auth-Token": auth_token}
    response = requests.get(url, headers=headers, verify=False)
    response.raise_for_status()
    return response.json().get("value", [])

def get_device(ome_host, auth_token, device_id):
    url = f"{ome_host}/api/DeviceService/Devices({device_id})"
    headers = {"X-Auth-Token": auth_token}
    response = requests.get(url, headers=headers, verify=False)
    response.raise_for_status()
    return response.json()

def get_device_warranty(ome_host, auth_token, device_id):
    url = f"{ome_host}/api/WarrantyService/Warranties?DeviceId={device_id}"
    headers = {"X-Auth-Token": auth_token}
    response = requests.get(url, headers=headers, verify=False)
    if response.status_code == 200:
        return response.json()
    return {}

def get_device_inventory(ome_host, auth_token, device_id):
    url = f"{ome_host}/api/DeviceService/Devices({device_id})/InventoryDetails"
    headers = {"X-Auth-Token": auth_token}
    response = requests.get(url, headers=headers, verify=False)
    if response.status_code == 200:
        return response.json()
    return {}

def do_discover(ome_host, ome_user, ome_pass):
    try:
        token = get_auth_token(ome_host, ome_user, ome_pass)
        devices = get_devices(ome_host, token)
        lld_data = []
        for device in devices:
            lld_data.append({
                "{#DEVICEID}": device.get("Id"),
                "{#DEVICENAME}": device.get("DeviceName"),
                "{#MODEL}": device.get("Model"),
                "{#DEVICESERVICETAG}": device.get("DeviceServiceTag"),
                "{#CHASSISSERVICETAG}": device.get("ChassisServiceTag")
            })
        print(json.dumps({"data": lld_data}, indent=2))
    except Exception as e:
        print(json.dumps({"error": str(e)}))

def get_full_device_info(ome_host, auth_token, device_id):
    headers = {"X-Auth-Token": auth_token}
    def get(endpoint):
        url = f"{ome_host}{endpoint}"
        r = requests.get(url, headers=headers, verify=False)
        if r.status_code == 200:
            return r.json()
        return {}

    return {
        "device": get(f"/api/DeviceService/Devices({device_id})"),
        "inventory": get(f"/api/DeviceService/Devices({device_id})/InventoryDetails"),
        "firmware": get(f"/api/DeviceService/Devices({device_id})/FirmwareInventory"),
        "subsystem_health": get(f"/api/DeviceService/Devices({device_id})/SubSystemHealth"),
        "hardware_logs": get(f"/api/DeviceService/Devices({device_id})/HardwareLogs"),
        "recent_activity": get(f"/api/DeviceService/Devices({device_id})/RecentActivity"),
        "warranty": get(f"/api/WarrantyService/Warranties?DeviceId={device_id}"),
        "licenses": get(f"/api/LicenseService/Devices({device_id})/Licenses"),
        "alerts": get(f"/api/AlertService/Alerts?$filter=DeviceId eq {device_id}"),
        "compliance": get(f"/api/ComplianceService/Devices({device_id})/ComplianceReports"),
        "power_metrics": get(f"/api/PowerManagerService/Devices({device_id})/Metrics"),
        "power_cap": get(f"/api/PowerManagerService/Devices({device_id})/PowerCap"),
        "supportassist_client": get(f"/api/SupportAssistService/Registration/ClientDetails"),
        "supportassist_cases": get(f"/api/SupportAssistService/Cases?$filter=DeviceId eq {device_id}"),
        "network": get(f"/api/DeviceService/Devices({device_id})/NetworkInterfaces"),
        "groups": get(f"/api/GroupService/Devices({device_id})/Groups"),
    }


def do_item(ome_host, ome_user, ome_pass, device_id):
    try:
        token = get_auth_token(ome_host, ome_user, ome_pass)
        device = get_device(ome_host, token, device_id)
        warranty = get_device_warranty(ome_host, token, device_id)
        days_remaining = None
        if isinstance(warranty, dict) and "value" in warranty and warranty["value"]:
            days_remaining = warranty["value"][0].get("DaysRemaining")
        print(json.dumps({
            "DeviceStatus": device.get("DeviceStatus"),
            "Health": device.get("Health"),
            "Model": device.get("Model"),
            "ServiceTag": device.get("ServiceTag"),
            "DaysRemainingWarranty": days_remaining
        }))
    except Exception as e:
        print(json.dumps({"error": str(e)}))

def find_device_id_by_name(ome_host, ome_user, ome_pass, device_name):
    token = get_auth_token(ome_host, ome_user, ome_pass)
    devices = get_devices(ome_host, token)
    for device in devices:
        if device.get("DeviceName") == device_name:
            return device.get("Id")
    return None

def do_all(ome_host, ome_user, ome_pass):
    try:
        token = get_auth_token(ome_host, ome_user, ome_pass)
        devices = get_devices(ome_host, token)
        all_data = []
        for device in devices:
            device_id = device.get("Id")
            inventory = get_device_inventory(ome_host, token, device_id)
            # Merge device info and inventory details
            combined = {
                "device_list_fields": device,
                "inventory_details": inventory
            }
            all_data.append(combined)
        print(json.dumps(all_data, indent=2))
    except Exception as e:
        print(json.dumps({"error": str(e)}))

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Dell OME Zabbix Discovery/Item Script")
    parser.add_argument('--ome-host', help='OME server URL (e.g. https://ome.example.com)', default=default_ome_host)
    parser.add_argument('--ome-user', help='OME username', default=default_ome_user)
    parser.add_argument('--ome-pass', help='OME password', default=default_ome_pass)
    group = parser.add_mutually_exclusive_group(required=True)
    group.add_argument('--discover', action='store_true', help='Run in discovery mode')
    group.add_argument('--item-id', metavar='DEVICE_ID', help='Get item metrics for DEVICE_ID')
    group.add_argument('--item-name', metavar='DEVICE_NAME', help='Get item metrics for DEVICE_NAME')
    group.add_argument('--all', action='store_true', help='Dump all fields for all devices')
    args = parser.parse_args()

    # Ensure all credentials are present
    if not args.ome_host or not args.ome_user or not args.ome_pass:
        print(json.dumps({"error": "OME host, username, and password must be provided (via arguments or zabbix_auth.py)"}))
        sys.exit(1)

    if args.discover:
        do_discover(args.ome_host, args.ome_user, args.ome_pass)
    elif args.item_id:
        do_item(args.ome_host, args.ome_user, args.ome_pass, args.item_id)
    elif args.item_name:
        device_id = find_device_id_by_name(args.ome_host, args.ome_user, args.ome_pass, args.item_name)
        if device_id:
            do_item(args.ome_host, args.ome_user, args.ome_pass, device_id)
        else:
            print(json.dumps({"error": f"Device with name '{args.item_name}' not found"}))
    elif args.all:
        do_all(args.ome_host, args.ome_user, args.ome_pass)

