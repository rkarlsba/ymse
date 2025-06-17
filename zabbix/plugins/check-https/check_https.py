#!/usr/bin/env python3
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker

import argparse
import http.client
import ssl
import certifi
import socket
import sys

def get_ip_addresses(host):
    """Return a list of (family, IP) tuples for all A and AAAA records."""
    try:
        infos = socket.getaddrinfo(host, 443, proto=socket.IPPROTO_TCP)
    except socket.gaierror as e:
        print(f"DNS lookup failed for {host}: {e}", file=sys.stderr)
        return []
    addresses = []
    seen = set()
    for info in infos:
        family, _, _, _, sockaddr = info
        ip = sockaddr[0]
        if (family, ip) not in seen:
            addresses.append((family, ip))
            seen.add((family, ip))
    return addresses

def get_final_status(ip, host, path, context, family, max_redirects=5):
    redirects = 0
    curr_host = host
    curr_path = path
    while redirects <= max_redirects:
        try:
            conn = http.client.HTTPSConnection(
                host=ip,
                port=443,
                context=context,
                timeout=5,
                source_address=None,
                # Use the original hostname for SNI and Host header
            )
            conn.set_tunnel(curr_host, 443)
        except Exception as e:
            return None, f"Connection error: {e}"

        try:
            conn.request("GET", curr_path, headers={'Host': curr_host})
            response = conn.getresponse()
        except Exception as e:
            conn.close()
            return None, f"Request error: {e}"

        # Handle 301/302 redirects
        if response.status in (301, 302):
            location = response.getheader('Location')
            if not location:
                conn.close()
                return None, "Redirect with no Location header"
            # Parse new host and path
            if location.startswith('https://'):
                location = location[8:]
                split = location.find('/')
                if split == -1:
                    curr_host = location
                    curr_path = '/'
                else:
                    curr_host = location[:split]
                    curr_path = location[split:]
            elif location.startswith('/'):
                curr_path = location
            else:
                # Relative path
                curr_path = '/' + location
            redirects += 1
            conn.close()
            continue
        status = response.status
        conn.close()
        return status, None
    return None, "Too many redirects"

def main():
    parser = argparse.ArgumentParser(description='HTTPS client that probes all DNS addresses (IPv4/IPv6) and confirms HTTP 200')
    parser.add_argument('host', help='The target HTTPS server (e.g., www.example.com)')
    parser.add_argument('--path', default='/', help='Path to request (default: /)')
    args = parser.parse_args()

    context = ssl.create_default_context(cafile=certifi.where())
    addresses = get_ip_addresses(args.host)
    if not addresses:
        print(f"No IP addresses found for {args.host}")
        sys.exit(1)

    for family, ip in addresses:
        famstr = "IPv4" if family == socket.AF_INET else "IPv6"
        try:
            status, err = get_final_status(ip, args.host, args.path, context, family)
            if status == 200:
                print(f"{args.host} ({ip}, {famstr}) returned HTTP 200 OK")
            elif status is not None:
                print(f"{args.host} ({ip}, {famstr}) returned HTTP {status}")
            else:
                print(f"{args.host} ({ip}, {famstr}) failed: {err}")
        except Exception as e:
            print(f"{args.host} ({ip}, {famstr}) failed: {e}")

if __name__ == '__main__':
    main()

