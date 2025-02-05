#!/usr/bin/env python3

import json
import sys

if __name__ == "__main__":
    json_filter = { "name": "Maintenance" }
    jsontest = {
        "groupid": "44",
        "name": "Maintenance",
        "hosts": [
            {
                "hostid": "13534",
                "host": "testlinux01.oslomet.no"
            },
            {
                "hostid": "13501",
                "host": "test-mecm-bbf.ada.hioa.no"
            }
        ]
    }

    gruppe = json_filter["name"]

    print(json.dumps(json_filter,indent=4))
    print(json.dumps(jsontest,indent=4))
