#!/usr/bin/env python3
# vim:ts=4:sw=4:sts=4:et:ai:si:fdm=marker

"""
Live UDP prefix analysis tool.

This script sniffs packets from a network interface, extracts UDP payloads,
counts prefix occurrences up to a chosen length, and reports entropy and
top prefixes. It can list available network interfaces, auto-pick a default,
limit capture by time or packet count, and write results to a file.
"""

import sys
import argparse
from collections import Counter
from math import log2
from pathlib import Path

from scapy.all import sniff, conf, get_if_list, UDP, Raw


# Description: Compute the Shannon entropy (in bits) from a Counter of values.
def entropy(counter):
    """
    Compute Shannon entropy in bits for the given value frequency counter.

    Args:
        counter (collections.Counter): A counter mapping values to counts.

    Returns:
        float: Entropy in bits (0.0 if counter is empty).
    """
    total = sum(counter.values())
    if total == 0:
        return 0.0

    e = 0.0
    for count in counter.values():
        p = count / total
        e -= p * log2(p)
    return e


# Description: Convenience wrapper to print to either stdout or a file handle.
def outprint(out, *args, **kwargs):
    """
    Print to the provided output handle.

    Args:
        out (IO): Output file-like object.
        *args: Values to print.
        **kwargs: Print keyword arguments.
    """
    print(*args, file=out, **kwargs)


# Description: List available network interfaces and mark the default interface.
def list_nics():
    """
    List available network interfaces and highlight the default interface.

    Exits with code 2 on errors.
    """
    try:
        nics = get_if_list()
    except Exception as e:
        print(f"Could not retrieve network interfaces: {e}", file=sys.stderr)
        sys.exit(2)

    default_iface = None
    try:
        default_iface = conf.iface
    except Exception:
        pass

    print("Available network interfaces:")
    for nic in nics:
        mark = " *" if nic == default_iface else ""
        print(f"  {nic}{mark}")
    if default_iface:
        print(f"\n'*' marks the default interface: {default_iface}")


# Description: Select a sensible default sniffing interface.
def select_default_iface():
    """
    Select a default interface to sniff on.

    Preference order:
    1) scapy's configured default interface (conf.iface)
    2) the first interface returned by get_if_list()

    Returns:
        str: The interface name.

    Raises:
        RuntimeError: If no interfaces are found.
    """
    if getattr(conf, "iface", None):
        return conf.iface
    nics = get_if_list()
    if not nics:
        raise RuntimeError("No network interfaces found")
    return nics[0]


# Description: Sniff live traffic and perform UDP payload prefix analysis.
def analyze_live(
    iface,
    timeout,
    count,
    bpf_filter,
    max_prefix,
    entropy_limit,
    min_pct,
    output_file,
    progress_every,
    promisc,
):
    """
    Sniff live packets on an interface and analyze UDP payload prefixes.

    Args:
        iface (str): Interface to sniff.
        timeout (float|None): Stop after this many seconds (<=0 or None for infinite).
        count (int|None): Stop after this many packets (<=0 or None for infinite).
        bpf_filter (str): BPF expression (e.g., "udp", "port 53"); empty for none.
        max_prefix (int): Max prefix length to analyze (bytes).
        entropy_limit (float): Stop at first prefix length where entropy exceeds this.
        min_pct (float): Minimum percentage threshold to display a prefix line.
        output_file (str|None): Write results to this file instead of stdout.
        progress_every (int): Print progress every N packets (0 disables).
        promisc (bool): Enable promiscuous mode if True.
    """
    prefix_stats = [Counter() for _ in range(max_prefix)]
    packets = 0
    udp_packets = 0
    payload_packets = 0

    def process_packet(pkt):
        nonlocal packets, udp_packets, payload_packets
        packets += 1

        if progress_every and packets % progress_every == 0:
            print(
                f"\rPackets={packets:,} UDP={udp_packets:,} Payload={payload_packets:,}",
                end="",
                flush=True,
            )

        if UDP not in pkt:
            return

        udp_packets += 1

        if Raw not in pkt:
            return

        payload = bytes(pkt[Raw].load)
        if not payload:
            return

        payload_packets += 1
        maxlen = min(max_prefix, len(payload))
        for n in range(1, maxlen + 1):
            prefix_stats[n - 1][payload[:n]] += 1

    sniff_timeout = None if (timeout is None or timeout <= 0) else timeout
    sniff_count = None if (count is None or count <= 0) else count

    try:
        sniff(
            iface=iface,
            prn=process_packet,
            store=False,
            filter=bpf_filter if bpf_filter else None,
            timeout=sniff_timeout,
            count=sniff_count,
            promisc=promisc,
        )
    except KeyboardInterrupt:
        print("\nInterrupted by user.", file=sys.stderr)

    print()  # end any in-place progress line

    if output_file:
        output_path = Path(output_file)
        if output_path.parent != Path("."):
            output_path.parent.mkdir(parents=True, exist_ok=True)
        out = open(output_path, "w", encoding="utf-8")
        should_close = True
    else:
        out = sys.stdout
        should_close = False

    try:
        outprint(out, "SUMMARY")
        outprint(out, "=======")
        outprint(out, f"Interface      : {iface}")
        outprint(out, f"Packets        : {packets:,}")
        outprint(out, f"UDP packets    : {udp_packets:,}")
        outprint(out, f"UDP payload    : {payload_packets:,}")
        outprint(out)

        outprint(out, "PREFIX ANALYSIS")
        outprint(out, "===============")

        for length in range(1, max_prefix + 1):
            counter = prefix_stats[length - 1]
            if not counter:
                break

            total = sum(counter.values())
            ent = entropy(counter)

            outprint(out)
            outprint(out, f"Prefix length {length}")
            outprint(out, "-" * 60)
            outprint(out, f"Entropy : {ent:.4f}")
            outprint(out, f"Unique  : {len(counter):,}")

            shown = False
            for value, c in counter.most_common():
                pct = c * 100.0 / total
                if pct < min_pct:
                    break
                shown = True
                outprint(out, f"{pct:8.3f}% {c:10d} {value.hex(' ')}")

            if not shown:
                outprint(out, "(no prefixes above threshold)")

            if ent > entropy_limit:
                outprint(out)
                outprint(
                    out,
                    f"Stopping at prefix length {length}: "
                    f"entropy exceeds {entropy_limit}",
                )
                break

    finally:
        if should_close:
            out.close()


# Description: Build the command-line interface and options.
def build_arg_parser():
    """
    Build and return the argparse.ArgumentParser for the CLI.

    Returns:
        argparse.ArgumentParser: Configured parser with all options.
    """
    p = argparse.ArgumentParser(
        description=(
            "Sniff UDP packets from a network interface and analyze payload prefixes "
            "with entropy computation."
        )
    )
    p.add_argument(
        "--iface",
        help="Network interface to sniff on (auto-selected if omitted).",
    )
    p.add_argument(
        "--list-nics",
        action="store_true",
        help="List available network interfaces and exit.",
    )
    p.add_argument(
        "--timeout",
        type=float,
        default=30.0,
        help="Stop after this many seconds (<=0 for infinite). Default: 30.0",
    )
    p.add_argument(
        "--count",
        type=int,
        default=0,
        help="Stop after this many packets (<=0 for infinite).",
    )
    p.add_argument(
        "--filter",
        dest="bpf_filter",
        default="",
        help='BPF filter (e.g., "udp", "port 53"). Empty for no filter.',
    )
    p.add_argument(
        "--max-prefix",
        type=int,
        default=8,
        help="Maximum prefix length (bytes) to analyze. Default: 8",
    )
    p.add_argument(
        "--entropy-limit",
        type=float,
        default=2.0,
        help="Stop at the first prefix length where entropy exceeds this. Default: 2.0",
    )
    p.add_argument(
        "--min-pct",
        type=float,
        default=0.1,
        help="Minimum percentage to display a prefix line. Default: 0.1",
    )
    p.add_argument(
        "--output",
        dest="output_file",
        help="Write results to this file instead of stdout.",
    )
    p.add_argument(
        "--progress-every",
        type=int,
        default=100000,
        help="Print progress every N packets (0 disables). Default: 100000",
    )
    p.add_argument(
        "--no-promisc",
        action="store_true",
        help="Disable promiscuous mode while sniffing.",
    )
    return p


# Description: Entry point for the CLI.
def main():
    """
    Parse arguments, optionally list interfaces, and run the live analysis.
    """
    parser = build_arg_parser()
    args = parser.parse_args()

    if args.list_nics:
        list_nics()
        return

    try:
        iface = args.iface or select_default_iface()
    except Exception as e:
        print(f"Could not determine network interface: {e}", file=sys.stderr)
        sys.exit(2)

    analyze_live(
        iface=iface,
        timeout=args.timeout,
        count=args.count,
        bpf_filter=args.bpf_filter,
        max_prefix=args.max_prefix,
        entropy_limit=args.entropy_limit,
        min_pct=args.min_pct,
        output_file=args.output_file,
        progress_every=args.progress_every,
        promisc=(not args.no_promisc),
    )


if __name__ == "__main__":
    main()

