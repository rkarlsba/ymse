#!/usr/bin/env python3

import requests
from auth_details import *

import urllib3
urllib3.disable_warnings(urllib3.exceptions.InsecureRequestWarning)

def get_auth_token():
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

def get_devices(auth_token):
    url = f"{ome_host}/api/DeviceService/Devices"
    headers = {"X-Auth-Token": auth_token}
    response = requests.get(url, headers=headers, verify=False)
    response.raise_for_status()
    return response.json().get("value", [])

def get_device_status(auth_token, device_id):
    url = f"{ome_host}/api/DeviceService/Devices({device_id})/SubSystemHealth"
    headers = {"X-Auth-Token": auth_token}
    response = requests.get(url, headers=headers, verify=False)
    if response.status_code == 200:
        return response.json()
    else:
        return {"Status": "Unknown"}

if __name__ == "__main__":
    try:
        token = get_auth_token()
        devices = get_devices(token)
        print("Device Statuses:")
        for device in devices:
            device_id = device.get("Id")
            device_name = device.get("DeviceName")
            status_info = get_device_status(token, device_id)
            health_status = status_info.get("Health", "Unknown")
            print(f"- {device_name} (ID: {device_id}): Health Status = {health_status}")
    except Exception as e:
        print(f"Error: {e}")

