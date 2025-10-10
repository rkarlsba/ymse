#!/usr/bin/env python3
# vim:ts=4:sw=4:sts=4:et:ai:si:fdm=marker
#
# check-soa.py
#
# Usage: check_soa.py [-h] -z ZONE -S SERVERS [-v[ -v[ -v …]]]
#
# Check if zone ZONE has the same serial across the given servers.
#
# Written by Roy Sigurd Karlsbakk <roysk@oslomet.no> with kind help fra Copilot
# and myself and I. Not copyrighted anything.

import argparse
import dns.message
import dns.query
import dns.resolver
import ipaddress
import re
import socket

def contains_ip(s):
    # Extract potential IP-like substrings
    candidates = re.findall(r'\b(?:[0-9]{1,3}\.){3}[0-9]{1,3}\b|\b[0-9a-fA-F:]+\b', s)
    for candidate in candidates:
        try:
            ipaddress.ip_address(candidate)
            return True
        except ValueError:
            continue
    return False

def resolve_hostname(hostname):
    try:
        return socket.gethostbyname(hostname)
    except socket.gaierror as e:
        print(f"[ERROR] Failed to resolve {hostname}: {e}")
        return None

def get_soa_serial(zone, dns_server, verbosity=0):
    if verbosity > 1:
        print(f"[INFO] Querying SOA record for zone '{zone}' using DNS server '{dns_server}'...")

    query = dns.message.make_query(zone, dns.rdatatype.SOA)
    try:
        if contains_ip(dns_server):
            if verbosity:
                print(f"dns_server \"{dns_server}\" is an IP address")
            ip = dns_server
        else:
            ip = resolve_hostname(dns_server)
            if verbosity:
                print(f"dns_server \"{dns_server}\" is not an IP address, but {ip} is")

        response = dns.query.udp(query, ip, timeout=5)
        for answer in response.answer:
            if answer.rdtype == dns.rdatatype.SOA:
                soa_record = answer[0]
                if verbosity >= 2:
                    print(f"[DEBUG] Full SOA record: {soa_record}")
                return soa_record.serial
        if verbosity > 0:
            print("[WARN] No SOA record found in the response.")
        return None
    except Exception as e:
        if verbosity > 0:
            print(f"[ERROR] Query failed: {e}")
        return None

if __name__ == "__main__":
    # Først argparse
    argparser = argparse.ArgumentParser()

    argparser.add_argument("-z", "--zone", type=str, required=True,
                           help="Zone from which the SOA is looked up")
    argparser.add_argument("-S", "--servers", type=str, required=True,
                            help="Comma-separated list of DNS servers to query ")
    argparser.add_argument("-v", "--verbose", action="count", default=0,
                           help="Increase verbosity level (e.g., -v, -vv, -vvvvvv…)")

    args = argparser.parse_args()

    server_list = []
    server_list.extend([s.strip() for s in args.servers.split(",")])

    serials = {}

    for server in server_list:
        serial = get_soa_serial(args.zone, server, args.verbose)
        if serial is not None:
            serials[server] = serial
            if args.verbose > 1:
                print(f"{server}: SOA serial = {serial}")
        else:
            print(f"{server}: SOA serial not found.")

    # Analyze serials
    unique_serials = set(serials.values())

    if len(unique_serials) == 1:
        print(f"OK: All servers are in sync. Serial: {next(iter(unique_serials))}")
    else:
        print("ERROR: Mismatch detected in SOA serials: ", end='')
        # Find the most common serial (assumed to be correct)
        from collections import Counter
        serial_counts = Counter(serials.values())
        most_common_serial, _ = serial_counts.most_common(1)[0]

        for server, serial in serials.items():
            if serial != most_common_serial:
                direction = "behind" if serial < most_common_serial else "ahead"
                print(f"Server {server} is {direction} (serial: {serial}, expected: {most_common_serial})")

