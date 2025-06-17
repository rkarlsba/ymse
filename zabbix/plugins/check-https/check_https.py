#!/usr/bin/env python

import argparse
import http.client
import sys

def main():
    parser = argparse.ArgumentParser(description='HTTPS checker for all addresses')
    parser.add_argument('host', help='The target HTTPS hostname (www.example.com)')
    parser.add_argument('--path', default='/', help='Path to request (default: /)')
    args = parser.parse_args()

    conn = http.client.HTTPSConnection(args.host)
    conn.request("GET", args.path)
    response = conn.getresponse()

    print("Status:", response.status)
    print("Reason:", response.reason)
    print("Body:", response.read().decode())

    conn.close()
    sys.exit(0)

if __name__ == "__main__":
    main()

