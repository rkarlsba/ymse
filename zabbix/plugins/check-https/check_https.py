#!/usr/bin/env python
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker
# {{{ gammalt
# import argparse
# import http.client
# import sys
# 
# def main():
#     parser = argparse.ArgumentParser(description='HTTPS checker for all addresses')
#     parser.add_argument('host', help='The target HTTPS hostname (www.example.com)')
#     parser.add_argument('--path', default='/', help='Path to request (default: /)')
#     args = parser.parse_args()
# 
#     conn = http.client.HTTPSConnection(args.host)
#     conn.request("GET", args.path)
#     response = conn.getresponse()
# 
#     print("Status:", response.status)
#     print("Reason:", response.reason)
#     print("Body:", response.read().decode())
# 
#     conn.close()
#     sys.exit(0)
# 
# if __name__ == "__main__":
#     main()
# }}}

import argparse
import http.client
import ssl
import certifi

def main():
    parser = argparse.ArgumentParser(description='HTTPS client using certifi CA bundle')
    parser.add_argument('host', help='The target HTTPS server (e.g., www.example.com)')
    parser.add_argument('--path', default='/', help='Path to request (default: /)')
    args = parser.parse_args()

    # Use certifi's CA bundle
    context = ssl.create_default_context(cafile=certifi.where())

    conn = http.client.HTTPSConnection(args.host, context=context)
    conn.request("GET", args.path)
    response = conn.getresponse()

    print("Status:", response.status)
    print("Reason:", response.reason)
    print("Body:", response.read().decode())

    conn.close()

if __name__ == '__main__':
    main()

