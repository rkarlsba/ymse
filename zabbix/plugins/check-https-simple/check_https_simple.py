#!/usr/bin/env python3

import argparse
import requests
import sys

def check_https(url):
    try:
        response = requests.get(url, timeout=10, verify=True)
        if response.status_code == 200:
            print("OK: HTTPS check successful")
            sys.exit(0)
        else:
            print(f"ERROR: HTTPS check failed with status {response.status_code}")
            sys.exit(1)
    except requests.exceptions.RequestException as e:
        print(f"ERROR: HTTPS check failed: {str(e)}")
        sys.exit(1)

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Check if HTTPS URL is reachable.')
    parser.add_argument('url', type=str, help='HTTPS URL to check')
    args = parser.parse_args()

    check_https(args.url)

