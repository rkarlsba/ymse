import argparse
import dns.resolver
import dns.query
import dns.message
import dns.rdatatype

def get_soa_serial(zone, dns_server, verbosity=0):
    if verbosity >= 1:
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
        if verbosity >= 1:
            print("[WARN] No SOA record found in the response.")
        return None
    except Exception as e:
        if verbosity >= 1:
            print(f"[ERROR] Query failed: {e}")
        return None

def main():
    parser = argparse.ArgumentParser(description="Query a DNS server for a zone's SOA serial number.")
    parser.add_argument("--zone", required=True, help="DNS zone to query (e.g., example.com)")
    parser.add_argument("--server", required=True, help="DNS server to query (e.g., 8.8.8.8)")
    parser.add_argument("-v", "--verbose", action="count", default=0,
                        help="Increase verbosity level (e.g., -v, -vv, -vvv)")

    args = parser.parse_args()
    serial = get_soa_serial(args.zone, args.server, args.verbose)
    if serial is not None:
        print(f"SOA serial: {serial}")
    else:
        print("SOA serial not found.")

if __name__ == "__main__":
    main()
