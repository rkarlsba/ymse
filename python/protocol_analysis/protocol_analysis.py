#!/usr/bin/env python3
# vim:ts=4:sw=4:sts=4:et:ai:si:fdm=marker

#!/usr/bin/env python3
# vim:ts=4:sw=4:sts=4:et:ai:si:fdm=marker
"""
Live prefix analysis tool for UDP/TCP payloads.

This script sniffs packets from a network interface (UDP, TCP, or both),
extracts payloads, counts prefix occurrences up to a chosen length, and reports
entropy and top prefixes. It can list available interfaces, auto-pick a default,
limit capture by time or packet count, write results to a file, and gracefully
handle backend/socket errors by retrying with safer settings (e.g., disabling
kernel BPF filters and promiscuous mode, switching backends).
"""

import sys
import argparse
import logging
from collections import Counter
from math import log2
from pathlib import Path

from scapy.all import sniff, conf, get_if_list, UDP, TCP, Raw


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


def outprint(out, *args, **kwargs):
    """
    Print to the provided output handle.

    Args:
        out (IO): Output file-like object.
        *args: Values to print.
        **kwargs: Print keyword arguments.
    """
    print(*args, file=out, **kwargs)


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


def set_backend(backend):
    """
    Force Scapy's sniffing backend.

    Args:
        backend (str): One of 'auto', 'pcap', or 'bpf'.
                       - 'pcap' forces libpcap backend (requires the 'libpcap' Python package).
                       - 'bpf' forces native BPF backend (BSD/macOS).
                       - 'auto' leaves Scapy's default behavior.

    Notes:
        If 'libpcap' Python package is not installed, 'pcap' may fail to activate.
    """
    backend = backend.lower()
    if backend == "pcap":
        try:
            conf.use_pcap = True
            print("Using libpcap backend (conf.use_pcap=True).")
        except Exception as e:
            print(f"Warning: could not enable libpcap backend: {e}", file=sys.stderr)
    elif backend == "bpf":
        try:
            conf.use_pcap = False
            print("Using native BPF backend (conf.use_pcap=False).")
        except Exception as e:
            print(f"Warning: could not force BPF backend: {e}", file=sys.stderr)
    else:
        # auto: do nothing, Scapy decides
        pass


class scapy_warning_capture(logging.Handler):
    """
    Capture Scapy runtime warnings to detect socket/backend failures.
    Sets flags when warnings indicating socket failures are emitted.
    """

    def __init__(self):
        super().__init__(level=logging.WARNING)
        self.socket_failed = False

    def emit(self, record):
        msg = record.getMessage()
        # Heuristic: detect libpcap/BPF socket failures
        if "failed with" in msg and "Socket" in msg:
            self.socket_failed = True


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
    proto,
    py_filter,
    backend,
):
    """
    Sniff live packets on an interface and analyze UDP/TCP payload prefixes.

    Args:
        iface (str): Interface to sniff.
        timeout (float|None): Stop after this many seconds (<=0 or None for infinite).
        count (int|None): Stop after this many packets (<=0 or None for infinite).
        bpf_filter (str): Kernel BPF filter (e.g., "udp", "tcp", "port 53"); empty for none.
        max_prefix (int): Max prefix length to analyze (bytes).
        entropy_limit (float): Stop at first prefix length where entropy exceeds this.
        min_pct (float): Minimum percentage threshold to display a prefix line.
        output_file (str|None): Write results to this file instead of stdout.
        progress_every (int): Print progress every N packets (0 disables).
        promisc (bool): Enable promiscuous mode if True.
        proto (str): 'udp', 'tcp', or 'both' to select which protocols to analyze.
        py_filter (bool): Apply a Python-level protocol filter (avoids kernel BPF filter).
        backend (str): 'auto', 'pcap', or 'bpf' initial backend selection.
    """
    prefix_stats = [Counter() for _ in range(max_prefix)]
    packets = 0
    udp_packets = 0
    tcp_packets = 0
    udp_payload_packets = 0
    tcp_payload_packets = 0

    want_udp = proto in ("udp", "both")
    want_tcp = proto in ("tcp", "both")

    def process_packet(pkt):
        nonlocal packets, udp_packets, tcp_packets, udp_payload_packets, tcp_payload_packets
        packets += 1

        if progress_every and packets % progress_every == 0:
            print(
                f"\rPackets={packets:,} UDP={udp_packets:,} TCP={tcp_packets:,} "
                f"UDP payload={udp_payload_packets:,} TCP payload={tcp_payload_packets:,}",
                end="",
                flush=True,
            )

        handled = False

        if want_udp and UDP in pkt:
            udp_packets += 1
            if Raw in pkt:
                payload = bytes(pkt[Raw].load)
                if payload:
                    udp_payload_packets += 1
                    handled = True
                    maxlen = min(max_prefix, len(payload))
                    for n in range(1, maxlen + 1):
                        prefix_stats[n - 1][payload[:n]] += 1

        if want_tcp and TCP in pkt:
            tcp_packets += 1
            if Raw in pkt:
                payload = bytes(pkt[Raw].load)
                if payload:
                    tcp_payload_packets += 1
                    handled = True
                    maxlen = min(max_prefix, len(payload))
                    for n in range(1, maxlen + 1):
                        prefix_stats[n - 1][payload[:n]] += 1

        return handled

    sniff_timeout = None if (timeout is None or timeout <= 0) else timeout
    sniff_count = None if (count is None or count <= 0) else count

    def run_sniff(backend_choice, use_py_filter, use_promisc, use_bpf_filter):
        """
        Run one sniff attempt with given settings, capturing scapy warnings.

        Returns:
            dict: {
                "exception": Exception|None,
                "socket_failed": bool,
                "packets_before": int,
                "packets_after": int,
            }
        """
        # Set backend
        set_backend(backend_choice)

        # Build filters
        if use_py_filter:
            if want_udp and want_tcp:
                lfilter = lambda p: (UDP in p) or (TCP in p)
            elif want_udp:
                lfilter = lambda p: UDP in p
            else:
                lfilter = lambda p: TCP in p
            active_bpf_filter = None
        else:
            lfilter = None
            # If the user didn't provide a BPF filter, synthesize one to reduce kernel load
            if use_bpf_filter:
                active_bpf_filter = bpf_filter if bpf_filter else (
                    "udp or tcp" if (want_udp and want_tcp)
                    else ("udp" if want_udp else "tcp")
                )
            else:
                active_bpf_filter = None

        # Capture scapy runtime warnings
        scapy_logger = logging.getLogger("scapy.runtime")
        handler = scapy_warning_capture()
        scapy_logger.addHandler(handler)

        packets_before = packets
        exc = None
        try:
            sniff(
                iface=iface,
                prn=process_packet,
                store=False,
                filter=active_bpf_filter,
                lfilter=lfilter,
                timeout=sniff_timeout,
                count=sniff_count,
                promisc=use_promisc,
            )
        except KeyboardInterrupt:
            print("\nInterrupted by user.", file=sys.stderr)
        except Exception as e:
            exc = e
        finally:
            scapy_logger.removeHandler(handler)

        return {
            "exception": exc,
            "socket_failed": handler.socket_failed,
            "packets_before": packets_before,
            "packets_after": packets,
        }

    # Attempt sequence: start with user choices, then progressively safer fallbacks
    attempts = [
        dict(backend_choice=backend, use_py_filter=py_filter, use_promisc=promisc, use_bpf_filter=True),
        # Avoid kernel BPF filter
        dict(backend_choice=backend, use_py_filter=True, use_promisc=promisc, use_bpf_filter=False),
        # Disable promiscuous mode
        dict(backend_choice=backend, use_py_filter=True, use_promisc=False, use_bpf_filter=False),
    ]

    # Backend swap fallback if needed
    alt_backend = None
    if backend == "pcap":
        alt_backend = "bpf"
    elif backend == "bpf":
        alt_backend = "pcap"
    else:
        # auto: try pcap first, then bpf
        alt_backend = "pcap"

    attempts.append(dict(backend_choice=alt_backend, use_py_filter=True, use_promisc=False, use_bpf_filter=False))

    last_error = None
    for idx, params in enumerate(attempts, 1):
        res = run_sniff(**params)
        if res["exception"]:
            last_error = res["exception"]
            print(f"Attempt {idx}: sniff error: {last_error}", file=sys.stderr)
            continue
        if res["socket_failed"]:
            print(f"Attempt {idx}: detected socket failure, retrying with safer settings...", file=sys.stderr)
            continue
        # If sniff captured at least one packet or we honored a non-infinite stop condition, accept results
        captured = res["packets_after"] - res["packets_before"]
        if captured > 0 or (sniff_timeout is not None or sniff_count is not None):
            break
        else:
            print(f"Attempt {idx}: no packets captured, retrying...", file=sys.stderr)

    # Output results
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
        outprint(out, f"TCP packets    : {tcp_packets:,}")
        outprint(out, f"UDP payload    : {udp_payload_packets:,}")
        outprint(out, f"TCP payload    : {tcp_payload_packets:,}")
        if last_error:
            outprint(out, f"Note: last sniff error: {last_error}")
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


def build_arg_parser():
    """
    Build and return the argparse.ArgumentParser for the CLI.

    Returns:
        argparse.ArgumentParser: Configured parser with all options.
    """
    p = argparse.ArgumentParser(
        description=(
            "Sniff UDP/TCP packets from a network interface and analyze payload prefixes "
            "with entropy computation. Automatically retries on backend/socket errors."
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
        help='Kernel BPF filter (e.g., "udp", "tcp", "port 53"). Empty for none.',
    )
    p.add_argument(
        "--py-filter",
        action="store_true",
        help="Apply a Python-level protocol filter (avoids kernel BPF filter).",
    )
    p.add_argument(
        "--proto",
        choices=["udp", "tcp", "both"],
        default="both",
        help="Which protocols to analyze. Default: both.",
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
    p.add_argument(
        "--backend",
        choices=["auto", "pcap", "bpf"],
        default="auto",
        help="Select Scapy sniffing backend. 'pcap' often avoids macOS BPF issues.",
    )
    # Backward-compatibility alias for earlier advice; maps to --proto udp + --py-filter
    p.add_argument(
        "--udp-only",
        action="store_true",
        help=argparse.SUPPRESS,
    )
    return p


def main():
    """
    Parse arguments, optionally list interfaces, and run the live analysis
    with automatic fallbacks on backend/socket failures.
    """
    parser = build_arg_parser()
    args = parser.parse_args()

    if args.list_nics:
        list_nics()
        return

    # Backward compatibility: --udp-only implies proto=udp and py_filter=True
    if args.udp_only:
        args.proto = "udp"
        args.py_filter = True

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
        proto=args.proto,
        py_filter=args.py_filter,
        backend=args.backend,
    )


if __name__ == "__main__":
    main()

