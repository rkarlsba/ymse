#!/usr/bin/env python3
# vim:ts=4:sw=4:sts=4:et:ai:si:fdm=marker

"""
Live prefix analysis tool for UDP/TCP payloads.

This script sniffs packets from a network interface (UDP, TCP, or both),
extracts payloads, counts prefix occurrences up to a chosen length, and reports
entropy and top prefixes. It can list available interfaces, auto-pick a default,
limit capture by time or packet count, write results to a file, and gracefully
handle backend/socket errors by retrying with safer settings (e.g., disabling
kernel BPF filters and promiscuous mode, switching backends). It uses an
AsyncSniffer plus a watchdog to avoid getting stuck mid-capture and bounds
the reporting phase to prevent stalls.
"""

import sys
import argparse
import logging
import warnings
import time
from collections import Counter
from math import log2
from pathlib import Path

from scapy.all import AsyncSniffer, conf, get_if_list, UDP, TCP, Raw


def entropy(counter):
    """
    Compute Shannon entropy in bits for the given value frequency counter.
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
    """
    print(*args, file=out, **kwargs)


def list_nics():
    """
    List available network interfaces and highlight the default interface.
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
    Select a default interface to sniff on (conf.iface or the first from get_if_list()).
    """
    if getattr(conf, "iface", None):
        return conf.iface
    nics = get_if_list()
    if not nics:
        raise RuntimeError("No network interfaces found")
    return nics[0]


def set_backend(backend, verbose=False):
    """
    Force Scapy's sniffing backend: 'auto', 'pcap', or 'bpf'.
    """
    backend = backend.lower()
    msg = None
    if backend == "pcap":
        try:
            conf.use_pcap = True
            msg = "Using libpcap backend (conf.use_pcap=True)."
        except Exception as e:
            print(f"Warning: could not enable libpcap backend: {e}", file=sys.stderr)
    elif backend == "bpf":
        try:
            conf.use_pcap = False
            msg = "Using native BPF backend (conf.use_pcap=False)."
        except Exception as e:
            print(f"Warning: could not force BPF backend: {e}", file=sys.stderr)
    if verbose and msg:
        print(msg)


class scapy_warning_capture(logging.Handler):
    """
    Capture Scapy runtime warnings to detect socket/backend failures and silence them.
    """
    def __init__(self):
        super().__init__(level=logging.WARNING)
        self.socket_failed = False

    def emit(self, record):
        msg = record.getMessage()
        if "Socket" in msg and "failed" in msg and "closed" in msg:
            self.socket_failed = True


class scapy_log_silencer:
    """
    Context manager that replaces scapy.runtime handlers to suppress and capture warnings.
    """
    def __init__(self):
        self.logger = logging.getLogger("scapy.runtime")
        self.prev_handlers = None
        self.prev_level = None
        self.prev_propagate = None
        self.handler = scapy_warning_capture()

    def __enter__(self):
        self.prev_handlers = list(self.logger.handlers)
        self.prev_level = self.logger.level
        self.prev_propagate = self.logger.propagate

        self.logger.handlers = [self.handler]
        self.logger.setLevel(logging.WARNING)
        self.logger.propagate = False

        self._warnings_cm = warnings.catch_warnings()
        self._warnings_cm.__enter__()
        warnings.filterwarnings(
            "ignore",
            message=r".*Socket .* failed .* It was closed\.*",
            category=Warning,
        )
        return self.handler

    def __exit__(self, exc_type, exc, tb):
        self.logger.handlers = self.prev_handlers
        self.logger.setLevel(self.prev_level)
        self.logger.propagate = self.prev_propagate
        self._warnings_cm.__exit__(exc_type, exc, tb)


def render_prefix_analysis(
    out,
    title,
    prefix_stats,
    max_prefix,
    min_pct,
    entropy_limit,
    top_k,
    no_entropy,
    analysis_timeout,
):
    """
    Render the prefix analysis for the given stats with bounds and safeguards.
    """
    start_t = time.monotonic()
    outprint(out, title)
    outprint(out, "=" * len(title))
    try:
        out.flush()
    except Exception:
        pass

    try:
        for length in range(1, max_prefix + 1):
            if analysis_timeout and (time.monotonic() - start_t) > analysis_timeout:
                outprint(out)
                outprint(out, f"(analysis aborted: exceeded {analysis_timeout}s reporting time)")
                break

            counter = prefix_stats[length - 1]
            if not counter:
                break

            total = sum(counter.values())
            ent = None if no_entropy else entropy(counter)

            outprint(out)
            outprint(out, f"Prefix length {length}")
            outprint(out, "-" * 60)
            if ent is not None:
                outprint(out, f"Entropy : {ent:.4f}")
            outprint(out, f"Unique  : {len(counter):,}")

            shown = False
            emitted = 0
            iterable = counter.most_common(top_k if top_k and top_k > 0 else None)
            for value, c in iterable:
                pct = c * 100.0 / total
                if pct < min_pct:
                    break
                shown = True
                emitted += 1
                outprint(out, f"{pct:8.3f}% {c:10d} {value.hex(' ')}")

            if not shown:
                outprint(out, "(no prefixes above threshold)")
            elif top_k and emitted >= top_k and emitted < len(counter):
                outprint(out, f"(truncated to top {top_k} entries)")

            if ent is not None and ent > entropy_limit:
                outprint(out)
                outprint(out, f"Stopping at prefix length {length}: entropy exceeds {entropy_limit}")
                break

        try:
            out.flush()
        except Exception:
            pass
    except Exception as e:
        outprint(out)
        outprint(out, f"(analysis aborted due to error: {e})")
        try:
            out.flush()
        except Exception:
            pass


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
    verbose,
    idle_timeout,
    top_k,
    no_entropy,
    analysis_timeout,
):
    """
    Sniff live packets and analyze UDP/TCP payload prefixes with fallbacks and watchdog.
    """
    prefix_stats = [Counter() for _ in range(max_prefix)]

    packets = 0
    udp_packets = 0
    tcp_packets = 0
    udp_payload_packets = 0
    tcp_payload_packets = 0

    want_udp = proto in ("udp", "both")
    want_tcp = proto in ("tcp", "both")

    last_progress = time.monotonic()

    def add_payload(payload):
        maxlen = min(max_prefix, len(payload))
        for n in range(1, maxlen + 1):
            prefix_stats[n - 1][payload[:n]] += 1

    def process_packet(pkt):
        nonlocal packets, udp_packets, tcp_packets, udp_payload_packets, tcp_payload_packets, last_progress
        packets += 1
        last_progress = time.monotonic()

        if progress_every and packets % progress_every == 0:
            print(
                f"\rPackets={packets:,} UDP={udp_packets:,} TCP={tcp_packets:,} "
                f"UDP payload={udp_payload_packets:,} TCP payload={tcp_payload_packets:,}",
                end="",
                flush=True,
            )

        if want_udp and UDP in pkt:
            udp_packets += 1
            if Raw in pkt:
                payload = bytes(pkt[Raw].load)
                if payload:
                    udp_payload_packets += 1
                    add_payload(payload)

        if want_tcp and TCP in pkt:
            tcp_packets += 1
            if Raw in pkt:
                payload = bytes(pkt[Raw].load)
                if payload:
                    tcp_payload_packets += 1
                    add_payload(payload)

    sniff_timeout = None if (timeout is None or timeout <= 0) else timeout
    sniff_count = None if (count is None or count <= 0) else count

    def run_sniff(backend_choice, use_py_filter, use_promisc, use_bpf_filter, attempt_no):
        set_backend(backend_choice, verbose=verbose)

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
            if use_bpf_filter:
                active_bpf_filter = bpf_filter if bpf_filter else (
                    "udp or tcp" if (want_udp and want_tcp)
                    else ("udp" if want_udp else "tcp")
                )
            else:
                active_bpf_filter = None

        if verbose:
            print(
                f"Attempt {attempt_no}: backend={backend_choice}, "
                f"py_filter={use_py_filter}, promisc={use_promisc}, "
                f"bpf_filter={'ON' if active_bpf_filter else 'OFF'}"
            )

        exc = None
        packets_before = packets

        with scapy_log_silencer() as cap:
            try:
                sniffer = AsyncSniffer(
                    iface=iface,
                    prn=process_packet,
                    store=False,
                    filter=active_bpf_filter,
                    lfilter=lfilter,
                    promisc=use_promisc,
                )
                sniffer.start()
            except Exception as e:
                exc = e
                return {
                    "exception": exc,
                    "socket_failed": cap.socket_failed,
                    "packets_before": packets_before,
                    "packets_after": packets,
                }

            start = time.monotonic()
            try:
                while True:
                    now = time.monotonic()
                    if sniff_timeout is not None and (now - start) >= sniff_timeout:
                        break
                    if sniff_count is not None and packets - packets_before >= sniff_count:
                        break
                    if idle_timeout and idle_timeout > 0 and (now - last_progress) >= idle_timeout:
                        cap.socket_failed = True
                        if verbose:
                            print(f"Attempt {attempt_no}: idle timeout reached ({idle_timeout}s) — aborting attempt.")
                        break
                    if not sniffer.running:
                        break
                    time.sleep(0.2)
            finally:
                try:
                    sniffer.stop()
                except Exception:
                    pass

        return {
            "exception": exc,
            "socket_failed": cap.socket_failed,
            "packets_before": packets_before,
            "packets_after": packets,
        }

    attempts = [
        dict(backend_choice=backend, use_py_filter=py_filter, use_promisc=promisc, use_bpf_filter=True),
        dict(backend_choice=backend, use_py_filter=True,     use_promisc=promisc, use_bpf_filter=False),
        dict(backend_choice=backend, use_py_filter=True,     use_promisc=False,   use_bpf_filter=False),
    ]
    if backend == "pcap":
        attempts.append(dict(backend_choice="bpf", use_py_filter=True, use_promisc=False, use_bpf_filter=False))
    elif backend == "bpf":
        attempts.append(dict(backend_choice="pcap", use_py_filter=True, use_promisc=False, use_bpf_filter=False))
    else:
        attempts.append(dict(backend_choice="pcap", use_py_filter=True, use_promisc=False, use_bpf_filter=False))

    last_error = None
    attempt_succeeded = False

    for idx, params in enumerate(attempts, 1):
        res = run_sniff(**params, attempt_no=idx)

        if res["exception"]:
            last_error = res["exception"]
            if verbose:
                print(f"Attempt {idx}: sniff error: {last_error}", file=sys.stderr)
            continue

        if res["socket_failed"]:
            if verbose:
                print(f"Attempt {idx}: detected socket failure or idle hang, retrying...", file=sys.stderr)
            continue

        captured = res["packets_after"] - res["packets_before"]

        # Accept the attempt if it captured any packets, or if we had a finite stop condition.
        if captured > 0 or (sniff_timeout is not None or sniff_count is not None):
            attempt_succeeded = True
            break

    if not attempt_succeeded and not last_error:
        last_error = RuntimeError("All sniff attempts failed or captured no packets.")

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
        try:
            out.flush()
        except Exception:
            pass

        render_prefix_analysis(
            out,
            title="PREFIX ANALYSIS",
            prefix_stats=prefix_stats,
            max_prefix=max_prefix,
            min_pct=min_pct,
            entropy_limit=entropy_limit,
            top_k=top_k,
            no_entropy=no_entropy,
            analysis_timeout=analysis_timeout,
        )
        try:
            out.flush()
        except Exception:
            pass

    except Exception as e:
        print(f"\n(Reporting aborted due to error: {e})", file=sys.stderr)
    finally:
        if should_close:
            out.close()


def build_arg_parser():
    """
    Build and return the argparse.ArgumentParser for the CLI.
    """
    p = argparse.ArgumentParser(
        description=(
            "Sniff UDP/TCP packets from a network interface and analyze payload prefixes "
            "with entropy computation. Automatically retries on backend/socket errors, "
            "uses a watchdog to avoid mid-capture hangs, and bounds the reporting phase."
        )
    )
    p.add_argument("--iface", help="Network interface to sniff on (auto-selected if omitted).")
    p.add_argument("--list-nics", action="store_true", help="List available network interfaces and exit.")
    p.add_argument("--timeout", type=float, default=30.0, help="Stop after this many seconds (<=0 for infinite). Default: 30.0")
    p.add_argument("--count", type=int, default=0, help="Stop after this many packets (<=0 for infinite).")
    p.add_argument("--filter", dest="bpf_filter", default="", help='Kernel BPF filter (e.g., "udp", "tcp", "port 53"). Empty for none.')
    p.add_argument("--py-filter", action="store_true", help="Apply a Python-level protocol filter (avoids kernel BPF filter).")
    p.add_argument("--proto", choices=["udp", "tcp", "both"], default="both", help="Which protocols to analyze. Default: both.")
    p.add_argument("--max-prefix", type=int, default=8, help="Maximum prefix length (bytes) to analyze. Default: 8")
    p.add_argument("--entropy-limit", type=float, default=2.0, help="Stop at the first prefix length where entropy exceeds this. Default: 2.0")
    p.add_argument("--min-pct", type=float, default=0.1, help="Minimum percentage to display a prefix line. Default: 0.1 (i.e., 0.1%).")
    p.add_argument("--output", dest="output_file", help="Write results to this file instead of stdout.")
    p.add_argument("--progress-every", type=int, default=100000, help="Print progress every N packets (0 disables). Default: 100000")
    p.add_argument("--no-promisc", action="store_true", help="Disable promiscuous mode while sniffing.")
    p.add_argument("--backend", choices=["auto", "pcap", "bpf"], default="auto", help="Select Scapy sniffing backend.")
    p.add_argument("--verbose", action="store_true", help="Show backend selection and retry steps.")
    p.add_argument("--idle-timeout", type=float, default=0.0, help="Abort an attempt if no packets for this many seconds (<=0 disables).")
    p.add_argument("--top-k", type=int, default=50, help="Limit lines per prefix length to top K (<=0 for unlimited). Default: 50")
    p.add_argument("--analysis-timeout", type=float, default=0.0, help="Hard cap (seconds) for the reporting section (<=0 disables).")
    p.add_argument("--no-entropy", action="store_true", help="Skip entropy computation during reporting.")
    # Backward-compatibility alias; maps to --proto udp + --py-filter
    p.add_argument("--udp-only", action="store_true", help=argparse.SUPPRESS)
    return p


def main():
    """
    Parse arguments, optionally list interfaces, and run the live analysis.
    """
    parser = build_arg_parser()
    args = parser.parse_args()

    if args.list_nics:
        list_nics()
        return

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
        verbose=args.verbose,
        idle_timeout=args.idle_timeout,
        top_k=args.top_k,
        no_entropy=args.no_entropy,
        analysis_timeout=args.analysis_timeout,
    )


if __name__ == "__main__":
    main()

