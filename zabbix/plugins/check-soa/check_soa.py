#!/usr/bin/env python3
# vim:ts=4:sw=4:sts=4:et:ai:si:fdm=marker

import argparse
import dns.resolver
import dns.query
import dns.message

def get_soa_serial(zone, dns_server, verbosity=0):
    if verbosity > 1:
        print(f"[INFO] Querying SOA record for zone '{zone}' using DNS server '{dns_server}'...")

    query = dns.message.make_query(zone, dns.rdatatype.SOA)
    try:
        response = dns.query.udp(query, dns_server, timeout=5)
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

    server_group = argparser.add_mutually_exclusive_group(required=True)
    server_group.add_argument("-s", "--server", type=str,
                              help="DNS server to query")
    server_group.add_argument("-S", "--servers", type=str,
                              help="Comma-separated list of DNS servers to query ")

    argparser.add_argument("-v", "--verbose", action="count", default=0,
                           help="Increase verbosity level (e.g., -v, -vv, -vvvvvv…)")

    args = argparser.parse_args()

    server_list = []
    
    if args.server:
        server_list.append(args.server)
    elif args.servers:
        server_list.extend([s.strip() for s in args.servers.split(",")])
   
    if args.verbose:
        print("server_list is ", server_list)
    for server in server_list:
        serial = get_soa_serial(args.zone, server, args.verbose)
        if serial is not None:
            if args.verbose > 1:
                print(f"{server}: SOA serial = {serial}")
        else:
            if args.verbose > 0:
                print(f"{server}: SOA serial not found.")

    print(serial)

