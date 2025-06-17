#!/usr/bin/env python3
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker

import argparse
import socket
import ssl
import certifi
import sys
import re

USER_AGENT = (
    "Mozilla/5.0 (Windows NT 10.0; Win64; x64) "
    "AppleWebKit/537.36 (KHTML, like Gecko) "
    "Chrome/114.0.0.0 Safari/537.36"
)

def get_ip_addresses(host):
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

def parse_http_status(response):
    # Extract HTTP status code from response bytes
    match = re.match(r"HTTP/\d+\.\d+\s+(\d+)", response)
    if match:
        return int(match.group(1))
    return None

def get_final_status(ip, host, path, family, context, max_redirects=5):
    curr_host = host
    curr_path = path
    redirects = 0
    while redirects <= max_redirects:
        try:
            # Create socket for the correct family (IPv4/IPv6)
            sock = socket.socket(family, socket.SOCK_STREAM)
            sock.settimeout(5)
            sock.connect((ip, 443))
            # Wrap with SSL, set SNI
            ssock = context.wrap_socket(sock, server_hostname=curr_host)
        except Exception as e:
            return None, f"Connection error: {e}"

        # Send HTTP request
        request = (
            f"GET {curr_path} HTTP/1.1\r\n"
            f"Host: {curr_host}\r\n"
            f"User-Agent: {USER_AGENT}\r\n"
            "Connection: close\r\n"
            "\r\n"
        )
        try:
            ssock.sendall(request.encode())
            response = b""
            while True:
                chunk = ssock.recv(4096)
                if not chunk:
                    break
                response += chunk
            ssock.close()
        except Exception as e:
            return None, f"Request error: {e}"

        # Decode HTTP status and handle redirects
        try:
            header_text = response.decode(errors="replace").split("\r\n\r\n", 1)[0]
            lines = header_text.split("\r\n")
            status_line = lines[0]
            status = parse_http_status(status_line)
            if status in (301, 302):
                location = None
                for line in lines[1:]:
                    if line.lower().startswith("location:"):
                        location = line.split(":", 1)[1].strip()
                        break
                if not location:
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
                    curr_path = '/' + location
                redirects += 1
                continue
            return status, None
        except Exception as e:
            return None, f"Parse error: {e}"
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
            status, err = get_final_status(ip, args.host, args.path, family, context)
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

