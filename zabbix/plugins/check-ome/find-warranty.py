#!/usr/bin/env python3

import argparse
import json
from datetime import datetime

def safe_load_json(filename):
    try:
        with open(filename, 'r') as f:
            data = json.load(f)
        return data
    except json.JSONDecodeError as e:
        print(f"Error decoding JSON in file {filename}: {e}")
        return None

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Dell OME Zabbix Discovery and Full Device Info Script")
    parser.add_argument('filenames', metavar='filenames', nargs='+',
                        help='Read JSON from given filename and output the date the warranty expires')
    args = parser.parse_args()

    # Les JSON-filen inn i variabelen 'warranty'

    for filename in args.filenames:
        hostdata = safe_load_json(filename)

        today = datetime.now()

        # Find the machine name, testing with json/log-core2-idrac.oslomet.no.json
        devicename = hostdata["device"]["DeviceName"]

        # Find the entry with the largest Id
        latest = max(hostdata["warranty"]["value"], key=lambda x: x["Id"])

        # Extract StartDate and EndDate
        warranty_end = latest["EndDate"]
        warranty_days = latest["DaysRemaining"]
        warranty_end_date = datetime.strptime(warranty_end, "%Y-%m-%d %H:%M:%S.%f")

        if today > warranty_end_date:
            print(f"{devicename}: The warranty is expired.")
        else:
            print(f"{devicename}: The warranty is still valid for {warranty_days} days.")

