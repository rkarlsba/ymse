#!/usr/bin/env python3
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker:tw=100

# check_https_all_addresses.py
# A simple HTTPS checker that instead of just checking the first IP address returned from a lookup,
# it checks them all.
#
# Written by Roy Sigurd Karlsbakk <roy@karlsbakk.net> with a whealthy bit of help from Perplexity
# AI.

import argparse
import socket
import ssl
import certifi
import sys
import re

user_agent = (
    "Mozilla/5.0 (Windows NT 10.0; Win64; x64) "
    "AppleWebKit/537.36 (KHTML, like Gecko) "
    "Chrome/114.0.0.0 Safari/537.36"
)

def get_ip_addresses(host, port):
    try:
        infos = socket.getaddrinfo(host, port, proto=socket.IPPROTO_TCP)
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

def get_final_status(ip, port, host, path, family, context, max_redirects=5, show_redirects=0,
                     verbose=0):
    curr_host = host
    curr_path = path
    curr_ip = ip
    curr_family = family
    redirects = 0

    while redirects <= max_redirects:
        if verbose:
            print(f"[1] [{redirects}] {curr_ip} {curr_host} {curr_path}")
        try:
            # Create socket for the correct family (IPv4/IPv6)
            sock = socket.socket(curr_family, socket.SOCK_STREAM)
            sock.settimeout(10)  # Increased timeout for IPv6
            if show_redirects and verbose:
                print(f'Connect to {curr_ip}:{port}')
            sock.connect((curr_ip, port))
            # Wrap with SSL, set SNI
            ssock = context.wrap_socket(sock, server_hostname=curr_host)
        except Exception as e:
            return None, f"Connection error: {e}"

        # Send HTTP request
        request = (
            f"GET {curr_path} HTTP/1.1\r\n"
            f"Host: {curr_host}\r\n"
            f"User-Agent: {user_agent}\r\n"
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
            if status in (301, 302, 303, 307, 308):
                location = None
                for line in lines[1:]:
                    if line.lower().startswith("location:"):
                        location = line.split(":", 1)[1].strip()
                        break
                if not location:
                    return None, "Redirect with no Location header"
                redirects += 1
                if show_redirects:
                    print(f'REDIRECT[{redirects}] to {location}')

                # Parse new host and path
                if location.startswith('https://'):
                    # Absolute URL with https
                    location = location[8:]
                    split = location.find('/')
                    if split == -1:
                        new_host = location
                        new_path = '/'
                    else:
                        new_host = location[:split]
                        new_path = location[split:]
                    if new_host != curr_host:
                        if verbose:
                            print(f"Host changed: {curr_host} → {new_host}")
                        # Re-resolve new host for updated IP and family
                        new_addresses = get_ip_addresses(new_host, port)
                        if not new_addresses:
                            return None, f"DNS lookup failed for redirect host {new_host}"
                        # Select first address matching current family if available
                        family_match = [addr for addr in new_addresses if addr[0] == curr_family]
                        if family_match:
                            curr_family, curr_ip = family_match[0]
                        else:
                            curr_family, curr_ip = new_addresses[0]
                        if verbose:
                            famstr = "IPv4" if curr_family == socket.AF_INET else "IPv6"
                            print(f"Resolved new IP: {curr_ip} ({famstr})")
                        curr_host = new_host
                    curr_path = new_path
                elif location.startswith('http://'):
                    # Absolute URL with http (not supported in this script)
                    return None, "Redirected to HTTP, not HTTPS"
                elif location.startswith('/'):
                    # Relative path
                    curr_path = location
                else:
                    # Other relative path
                    curr_path = '/' + location
                continue
            return status, None
        except Exception as e:
            return None, f"Parse error: {e}"
    if verbose:
        print(f"[2] [{redirects}] {curr_ip} {curr_host} {curr_path}")
    return None, "Too many redirects"

def main():
    ok_count = 0
    err_count = 0
    ignore_cert = 0
    ipv4_ok_count = 0
    ipv6_ok_count = 0
    ipv4_err_count = 0
    ipv6_err_count = 0
    show_redirects = 0
    verbose = 0
    port = 443

    parser = argparse.ArgumentParser(description='HTTPS client that probes all DNS addresses (IPv4/IPv6) and confirms HTTP 200')
    parser.add_argument('host', help='The target HTTPS server (e.g., www.example.com)')
    parser.add_argument('-p', '--path', default='/', help='Path to request (default: /)')
    parser.add_argument('-P', '--port', type=int, default=port, help=f'HTTPS port chosen (default {port})')
    parser.add_argument('-i', '--insecure', action='store_true', help='Ignore invalid HTTPS certificates')
    parser.add_argument('-v', '--verbose', action='count', default=0, help='Be verbose (more -v\'s for even more verbosity)')
    parser.add_argument('-r', '--show-redirects', action='store_true', help='Show redirects')
    args = parser.parse_args()

    verbose = args.verbose
    ignore_cert = args.insecure
    show_redirects = args.show_redirects
    port = args.port

    context = ssl.create_default_context(cafile=certifi.where())
    if ignore_cert:
        context.check_hostname = False
        context.verify_mode = ssl.CERT_NONE
    addresses = get_ip_addresses(args.host, port)
    if not addresses:
        print(f"No IP addresses found for {args.host}", file=sys.stderr)
        sys.exit(1)

    for family, ip in addresses:
        famstr = "IPv4" if family == socket.AF_INET else "IPv6"
        try:
            status, err = get_final_status(ip, port, args.host, args.path, family, context, 5,
                                           show_redirects, verbose)
            if verbose:
                print(f"STATUS/ERR: [{status}] '{err}'")
            if status == 200:
                ok_count += 1
                if famstr == "IPv4":
                    ipv4_ok_count +=1
                else:
                    ipv6_ok_count +=1
                if verbose:
                    print(f"{args.host} ({ip}, {famstr}) returned HTTP 200 OK")
            elif status is not None:
                err_count += 1
                if famstr == "IPv4":
                    ipv4_err_count +=1
                else:
                    ipv6_err_count +=1
                if verbose:
                    print(f"{args.host} ({ip}, {famstr}) returned HTTP {status}")
                else:
                    print(f"{args.host} ({ip}, {famstr}) failed: HTTP {status}", file=sys.stderr)
            else:
                err_count += 1
                if famstr == "IPv4":
                    ipv4_err_count +=1
                else:
                    ipv6_err_count +=1
                print(f"{args.host} ({ip}, {famstr}) failed: {err}", file=sys.stderr)
        except Exception as e:
            err_count += 1
            if famstr == "IPv4":
                ipv4_err_count +=1
            else:
                ipv6_err_count +=1
            print(f"{args.host} ({ip}, {famstr}) failed: {e}", file=sys.stderr)

    if err_count == 0 and ok_count > 0:
        status = 'OK'
    elif err_count > 0 and ok_count > 0:
        status = 'WARN'
    else:
        status = 'ERROR'

    print(f"{status}: HTTPS check for {args.host} complete with {ok_count} (IPv4: {ipv4_ok_count}, IPv6: {ipv6_ok_count}) successes and {err_count} (IPv4: {ipv4_err_count}, IPv6: {ipv6_err_count}) failures")

if __name__ == '__main__':
    main()

