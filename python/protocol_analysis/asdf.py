#!/usr/bin/env python3
# vim:ts=4:sw=4:sts=4:et:ai:si:fdm=marker

"""
Live prefix analysis for UDP/TCP payloads with reliable capture.

Sniffs packets (UDP, TCP, or both), extracts payloads, counts prefix occurrences
up to a chosen length, and reports entropy and top prefixes.

Capture backends:
- tcpdump: runs the system tcpdump and parses its pcap/pcapng stream (recommended on macOS).
- scapy: uses Scapy's sniff() (may be flaky on some macOS/Python combos).

You can list interfaces, auto-pick a default, limit by time/packet count, choose
backend, filter by IP and/or port, and filter out small/irrelevant hits in the report.
"""

import sys
import argparse
import time
import subprocess
import shlex
from collections import Counter
from math import log2
from pathlib import Path

from scapy.all import sniff, conf, get_if_list, UDP, TCP, Raw
from scapy.layers.l2 import Ether
from scapy.layers.inet import IP
from scapy.layers.inet6 import IPv6
from scapy.utils import RawPcapReader, RawPcapNgReader

# Common pcap DLT (linktype) constants we need
DLT_NULL = 0          # BSD loopback
DLT_EN10MB = 1        # Ethernet
DLT_RAW = 12          # Raw IP (no L2)
DLT_LOOP = 108        # Another loopback variant
DLT_PKTAP = 248       # Apple PKTAP (macOS)

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

    default_iface = getattr(conf, "iface", None)
    default_str = str(default_iface) if default_iface is not None else None

    print("Available network interfaces:")
    for nic in nics:
        name = str(nic)
        mark = " *" if default_str and name == default_str else ""
        print(f"  {name}{mark}")
    if default_str:
        print(f"\n'*' marks the default interface: {default_str}")


def select_default_iface():
    """
    Select a default interface to sniff on (conf.iface or the first from get_if_list()).
    Returns the interface name as a string.
    """
    if getattr(conf, "iface", None):
        return str(conf.iface)
    nics = get_if_list()
    if not nics:
        raise RuntimeError("No network interfaces found")
    return str(nics[0])


def build_bpf_expr(proto, base_filter, ips, src_ips, dst_ips, ports, src_ports, dst_ports):
    """
    Build a BPF expression string combining proto, ip/port filters, and an optional base filter.
    For IP filters, include (ip or ip6) guard so it works for both families.
    """
    clauses = []

    # protocol clause
    if proto == "both":
        clauses.append("(udp or tcp)")
    elif proto in ("udp", "tcp"):
        clauses.append(proto)

    have_ip_filters = bool(ips or src_ips or dst_ips)

    # IP clauses
    ip_terms = []
    for ip in (ips or []):
        ip_terms.append(f"host {ip}")
    for ip in (src_ips or []):
        ip_terms.append(f"src host {ip}")
    for ip in (dst_ips or []):
        ip_terms.append(f"dst host {ip}")
    if ip_terms:
        ip_clause = "(" + " or ".join(ip_terms) + ")"
        clauses.append(f"(ip or ip6) and {ip_clause}")

    # port clauses
    port_terms = []
    for p in (ports or []):
        port_terms.append(f"port {p}")
    for p in (src_ports or []):
        port_terms.append(f"src port {p}")
    for p in (dst_ports or []):
        port_terms.append(f"dst port {p}")
    if port_terms:
        clauses.append("(" + " or ".join(port_terms) + ")")

    expr = " and ".join(clauses) if clauses else ""
    if base_filter:
        expr = f"({expr}) and ({base_filter})" if expr else base_filter
    return expr


def build_lfilter(proto, ips, src_ips, dst_ips, ports, src_ports, dst_ports):
    """
    Build a Python-level filter function equivalent to the BPF expression.
    Matches IPv4/IPv6 and UDP/TCP according to options.
    """
    want_udp = proto in ("udp", "both")
    want_tcp = proto in ("tcp", "both")

    ips = set(ips or [])
    src_ips = set(src_ips or [])
    dst_ips = set(dst_ips or [])
    ports = set(int(p) for p in (ports or []))
    src_ports = set(int(p) for p in (src_ports or []))
    dst_ports = set(int(p) for p in (dst_ports or []))

    def ip_match(pkt):
        s = d = None
        if IP in pkt:
            s = pkt[IP].src
            d = pkt[IP].dst
        elif IPv6 in pkt:
            s = pkt[IPv6].src
            d = pkt[IPv6].dst

        # if no IP layer and any ip filters exist, fail
        have_ip_filters = ips or src_ips or dst_ips
        if have_ip_filters and s is None and d is None:
            return False

        # match logic: if any ips set => host matches either src or dst
        if ips and not (s in ips or d in ips):
            return False
        if src_ips and not (s in src_ips):
            return False
        if dst_ips and not (d in dst_ips):
            return False
        return True

    def port_match(pkt):
        have_port_filters = ports or src_ports or dst_ports
        if not have_port_filters:
            return True

        sport = dport = None
        if UDP in pkt:
            sport = int(pkt[UDP].sport)
            dport = int(pkt[UDP].dport)
        elif TCP in pkt:
            sport = int(pkt[TCP].sport)
            dport = int(pkt[TCP].dport)
        else:
            return False

        if ports and not (sport in ports or dport in ports):
            return False
        if src_ports and not (sport in src_ports):
            return False
        if dst_ports and not (dport in dst_ports):
            return False
        return True

    def proto_match(pkt):
        if want_udp and UDP in pkt:
            return True
        if want_tcp and TCP in pkt:
            return True
        return False

    def lf(pkt):
        return proto_match(pkt) and ip_match(pkt) and port_match(pkt)

    return lf


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
    min_count,
    coverage_pct,
    show_others,
):
    """
    Render the prefix analysis with bounds and filters.

    min_pct is a percentage threshold (e.g., 0.5 means 0.5%%).
    min_count requires at least N occurrences for an entry to be shown.
    coverage_pct keeps entries until cumulative coverage reaches this percent.
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

            items = counter.most_common()
            emitted = 0
            covered = 0.0
            printed_count_sum = 0

            for value, c in items:
                pct = (c * 100.0 / total) if total else 0.0

                # Apply thresholds and caps
                if c < min_count:
                    continue
                if pct < min_pct:
                    continue
                if coverage_pct and covered >= coverage_pct:
                    continue
                if top_k and top_k > 0 and emitted >= top_k:
                    continue

                emitted += 1
                covered += pct
                printed_count_sum += c
                outprint(out, f"{pct:8.3f}% {c:10d} {value.hex(' ')}")

            if emitted == 0:
                outprint(out, "(no prefixes above threshold)")
            elif show_others:
                others_count = max(0, total - printed_count_sum)
                others_entries = max(0, len(counter) - emitted)
                pct_rest = (others_count * 100.0 / total) if total else 0.0
                if others_entries > 0 and pct_rest > 0.0:
                    outprint(out, f"(others: {others_entries} entries, {pct_rest:.3f}% cumulative)")

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


def analyze_packets_from_bytes(raw_bytes, linktype, max_prefix, prefix_stats, proto, counters):
    """
    Parse a single raw frame using linktype and update stats (used by tcpdump backend).
    """
    want_udp = proto in ("udp", "both")
    want_tcp = proto in ("tcp", "both")

    pkt = parse_by_linktype(raw_bytes, linktype)
    if pkt is None:
        return

    if want_udp and UDP in pkt:
        counters["udp_packets"] += 1
        if Raw in pkt:
            payload = bytes(pkt[Raw].load)
            if payload:
                counters["udp_payload_packets"] += 1
                maxlen = min(max_prefix, len(payload))
                for n in range(1, maxlen + 1):
                    prefix_stats[n - 1][payload[:n]] += 1

    if want_tcp and TCP in pkt:
        counters["tcp_packets"] += 1
        if Raw in pkt:
            payload = bytes(pkt[Raw].load)
            if payload:
                counters["tcp_payload_packets"] += 1
                maxlen = min(max_prefix, len(payload))
                for n in range(1, maxlen + 1):
                    prefix_stats[n - 1][payload[:n]] += 1


def _parse_ip_or_ipv6(buf: bytes):
    if not buf:
        return None
    v = (buf[0] >> 4) & 0xF
    if v == 4:
        return IP(buf)
    if v == 6:
        return IPv6(buf)
    # Fallback: try IP first, then IPv6
    try:
        return IP(buf)
    except Exception:
        try:
            return IPv6(buf)
        except Exception:
            return None


def parse_by_linktype(raw: bytes, linktype: int):
    """
    Return a scapy Packet from raw bytes based on linktype, or None if unknown.
    Handles Ethernet, RAW IP, NULL/LOOP (skip 4-byte af header), and PKTAP (skip variable header).
    """
    try:
        if linktype == DLT_EN10MB:
            return Ether(raw)
        elif linktype == DLT_RAW:
            return _parse_ip_or_ipv6(raw)
        elif linktype in (DLT_NULL, DLT_LOOP):
            if len(raw) >= 4:
                return _parse_ip_or_ipv6(raw[4:])
            return None
        elif linktype == DLT_PKTAP:
            # PKTAP header: first uint32 is total header length (little-endian)
            if len(raw) >= 4:
                hdr_len = int.from_bytes(raw[:4], "little", signed=False)
                if 0 <= hdr_len <= len(raw):
                    return parse_by_linktype(raw[hdr_len:], DLT_RAW)  # underlying is usually RAW or Ethernet
            return None
        else:
            # Try Ethernet, then RAW
            try:
                return Ether(raw)
            except Exception:
                return _parse_ip_or_ipv6(raw)
    except Exception:
        return None


def capture_via_tcpdump(
    iface,
    bpf_filter,
    timeout,
    count,
    promisc,
    proto,
    max_prefix,
    prefix_stats,
    progress_every,
):
    """
    Run tcpdump and parse its pcap or pcapng stream from stdout.
    Updates prefix_stats and returns counters dict.
    """
    counters = {
        "packets": 0,
        "udp_packets": 0,
        "tcp_packets": 0,
        "udp_payload_packets": 0,
        "tcp_payload_packets": 0,
    }

    iface_str = str(iface)

    cmd = ["tcpdump", "-i", iface_str, "-U", "-w", "-", "-n", "-s", "0"]
    if promisc is False:
        cmd.append("-p")
    if count and count > 0:
        cmd += ["-c", str(count)]

    # Split BPF expression into tokens so subprocess doesn't need a shell
    if bpf_filter:
        cmd += shlex.split(bpf_filter)
    else:
        # Default based on proto
        cmd += shlex.split("udp or tcp" if proto == "both" else proto)

    proc = None
    killer = None
    try:
        proc = subprocess.Popen(
            cmd,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            bufsize=0,
        )

        # Kill tcpdump after timeout (if specified)
        if timeout and timeout > 0:
            import threading
            def _kill():
                try:
                    proc.terminate()
                except Exception:
                    pass
            killer = threading.Timer(timeout, _kill)
            killer.daemon = True
            killer.start()

        # Peek first 4 bytes to detect pcap vs pcapng
        first4 = proc.stdout.read(4)
        if not first4:
            return counters  # nothing captured
        magic = first4

        class prefixed_stream:
            def __init__(self, head, stream):
                self.head = head
                self.pos = 0
                self.stream = stream
            def read(self, n=-1):
                # Serve from the head buffer first
                if self.pos < len(self.head):
                    if n < 0:
                        data = self.head[self.pos:] + self._read_silent(n)
                        self.pos = len(self.head)
                        return data
                    take = min(n, len(self.head) - self.pos)
                    data = self.head[self.pos:self.pos+take]
                    self.pos += take
                    if n > take:
                        tail = self._read_silent(n - take)
                        return data + (tail or b"")
                    return data
                return self._read_silent(n)
            def _read_silent(self, n):
                try:
                    return self.stream.read(n)
                except ValueError:
                    # Underlying pipe is closed; behave like EOF
                    return b""
            def close(self):
                try:
                    self.stream.close()
                except Exception:
                    pass

        stream = prefixed_stream(first4, proc.stdout)

        # Choose reader and obtain linktype, then iterate once
        if magic == b"\x0a\x0d\x0d\x0a":  # pcapng
            reader = RawPcapNgReader(stream)
            for pkt_data, meta in reader:
                counters["packets"] += 1
                if progress_every and counters["packets"] % progress_every == 0:
                    print(
                        f"\rPackets={counters['packets']:,} UDP={counters['udp_packets']:,} "
                        f"TCP={counters['tcp_packets']:,} "
                        f"UDP payload={counters['udp_payload_packets']:,} "
                        f"TCP payload={counters['tcp_payload_packets']:,}",
                        end="", flush=True
                    )
                lt = meta.get("linktype", DLT_EN10MB)
                analyze_packets_from_bytes(
                    pkt_data,
                    linktype=lt,
                    max_prefix=max_prefix,
                    prefix_stats=prefix_stats,
                    proto=proto,
                    counters=counters,
                )
            try:
                reader.close()
            except Exception:
                pass
        else:
            reader = RawPcapReader(stream)
            # For classic pcap, the linktype is per-file
            file_linktype = getattr(reader, "linktype", DLT_EN10MB)
            for pkt_data, _ in reader:
                counters["packets"] += 1
                if progress_every and counters["packets"] % progress_every == 0:
                    print(
                        f"\rPackets={counters['packets']:,} UDP={counters['udp_packets']:,} "
                        f"TCP={counters['tcp_packets']:,} "
                        f"UDP payload={counters['udp_payload_packets']:,} "
                        f"TCP payload={counters['tcp_payload_packets']:,}",
                        end="", flush=True
                    )
                analyze_packets_from_bytes(
                    pkt_data,
                    linktype=file_linktype,
                    max_prefix=max_prefix,
                    prefix_stats=prefix_stats,
                    proto=proto,
                    counters=counters,
                )
            try:
                reader.close()
            except Exception:
                pass

        # Drain and ignore stderr (tcpdump stats)
        try:
            if proc.stderr:
                proc.stderr.read()
        except Exception:
            pass

    finally:
        if killer:
            try:
                killer.cancel()
            except Exception:
                pass
        if proc:
            try:
                proc.terminate()
            except Exception:
                pass
            try:
                proc.wait(timeout=1)
            except Exception:
                pass

    print()
    return counters


def capture_via_scapy(
    iface,
    bpf_filter,
    timeout,
    count,
    promisc,
    proto,
    max_prefix,
    prefix_stats,
    progress_every,
    lfilter,
):
    """
    Capture using Scapy's sniff() and update stats. Returns counters dict.
    """
    counters = {
        "packets": 0,
        "udp_packets": 0,
        "tcp_packets": 0,
        "udp_payload_packets": 0,
        "tcp_payload_packets": 0,
    }

    want_udp = proto in ("udp", "both")
    want_tcp = proto in ("tcp", "both")

    def add_payload(payload):
        maxlen = min(max_prefix, len(payload))
        for n in range(1, maxlen + 1):
            prefix_stats[n - 1][payload[:n]] += 1

    def process_packet(pkt):
        counters["packets"] += 1

        if progress_every and counters["packets"] % progress_every == 0:
            print(
                f"\rPackets={counters['packets']:,} UDP={counters['udp_packets']:,} "
                f"TCP={counters['tcp_packets']:,} "
                f"UDP payload={counters['udp_payload_packets']:,} "
                f"TCP payload={counters['tcp_payload_packets']:,}",
                end="", flush=True
            )

        if want_udp and UDP in pkt:
            counters["udp_packets"] += 1
            if Raw in pkt:
                payload = bytes(pkt[Raw].load)
                if payload:
                    counters["udp_payload_packets"] += 1
                    add_payload(payload)

        if want_tcp and TCP in pkt:
            counters["tcp_packets"] += 1
            if Raw in pkt:
                payload = bytes(pkt[Raw].load)
                if payload:
                    counters["tcp_payload_packets"] += 1
                    add_payload(payload)

    sniff_timeout = None if (timeout is None or timeout <= 0) else timeout
    sniff_count = None if (count is None or count <= 0) else count

    try:
        sniff(
            iface=str(iface),
            prn=process_packet,
            store=False,
            filter=bpf_filter if bpf_filter else None,
            lfilter=lfilter,
            timeout=sniff_timeout,
            count=sniff_count,
            promisc=promisc,
        )
    except KeyboardInterrupt:
        print("\nInterrupted by user.", file=sys.stderr)
    except Exception as e:
        print(f"\nSniff error: {e}", file=sys.stderr)

    print()
    return counters


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
    backend,
    top_k,
    no_entropy,
    analysis_timeout,
    min_count,
    coverage,
    show_others,
    ips,
    src_ips,
    dst_ips,
    ports,
    src_ports,
    dst_ports,
    py_filter,
):
    """
    Select capture backend, run capture, and render results.
    """
    prefix_stats = [Counter() for _ in range(max_prefix)]

    # Build filters
    bpf_expr = build_bpf_expr(proto, bpf_filter, ips, src_ips, dst_ips, ports, src_ports, dst_ports)
    lfilter = build_lfilter(proto, ips, src_ips, dst_ips, ports, src_ports, dst_ports) if py_filter else None

    # Decide backend(s)
    backends_to_try = []
    b = backend.lower()
    if b == "auto":
        if sys.platform == "darwin":
            backends_to_try = ["tcpdump", "scapy"]
        else:
            backends_to_try = ["scapy", "tcpdump"]
    elif b in ("scapy", "tcpdump"):
        backends_to_try = [b]
    else:
        backends_to_try = ["tcpdump"]

    counters = None
    last_error = None

    for b in backends_to_try:
        if b == "scapy":
            counters = capture_via_scapy(
                iface=iface,
                bpf_filter=bpf_expr if not py_filter else "",  # avoid kernel filter if py_filter is on
                timeout=timeout,
                count=count,
                promisc=promisc,
                proto=proto,
                max_prefix=max_prefix,
                prefix_stats=prefix_stats,
                progress_every=progress_every,
                lfilter=lfilter,
            )
            # If we captured nothing, try next backend
            if counters["packets"] == 0:
                last_error = RuntimeError("scapy backend captured 0 packets")
                continue
            break
        else:
            counters = capture_via_tcpdump(
                iface=iface,
                bpf_filter=bpf_expr,
                timeout=timeout,
                count=count,
                promisc=promisc,
                proto=proto,
                max_prefix=max_prefix,
                prefix_stats=prefix_stats,
                progress_every=progress_every,
            )
            if counters["packets"] == 0:
                last_error = RuntimeError("tcpdump backend captured 0 packets")
                # try next backend if any
                continue
            break

    # Prepare output
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
        if counters is None:
            counters = {
                "packets": 0,
                "udp_packets": 0,
                "tcp_packets": 0,
                "udp_payload_packets": 0,
                "tcp_payload_packets": 0,
            }

        outprint(out, "SUMMARY")
        outprint(out, "=======")
        outprint(out, f"Interface      : {iface}")
        outprint(out, f"Packets        : {counters['packets']:,}")
        outprint(out, f"UDP packets    : {counters['udp_packets']:,}")
        outprint(out, f"TCP packets    : {counters['tcp_packets']:,}")
        outprint(out, f"UDP payload    : {counters['udp_payload_packets']:,}")
        outprint(out, f"TCP payload    : {counters['tcp_payload_packets']:,}")
        if last_error:
            outprint(out, f"Note: {last_error}")
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
            min_count=min_count,
            coverage_pct=coverage,
            show_others=show_others,
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
            "Sniff UDP/TCP packets from a network interface and analyze payload prefixes. "
            "On macOS, --backend tcpdump (or --backend auto) is recommended for stability."
        )
    )
    p.add_argument("--iface", help="Network interface to sniff on (auto-selected if omitted).")
    p.add_argument("--list-nics", action="store_true", help="List available network interfaces and exit.")
    p.add_argument("--timeout", type=float, default=30.0, help="Stop after this many seconds (<=0 for infinite). Default: 30.0")
    p.add_argument("--count", type=int, default=0, help="Stop after this many packets (<=0 for infinite).")
    p.add_argument("--proto", choices=["udp", "tcp", "both"], default="both", help="Which protocols to analyze. Default: both.")

    # IP/port filters (can be repeated)
    p.add_argument("--ip", action="append", help="Filter for host (source or destination). May be repeated.")
    p.add_argument("--src-ip", action="append", help="Filter for source host. May be repeated.")
    p.add_argument("--dst-ip", action="append", help="Filter for destination host. May be repeated.")
    p.add_argument("--port", action="append", help="Filter for port (source or destination). May be repeated.")
    p.add_argument("--src-port", action="append", help="Filter for source port. May be repeated.")
    p.add_argument("--dst-port", action="append", help="Filter for destination port. May be repeated.")

    # Backend and filtering mode
    p.add_argument("--backend", choices=["auto", "scapy", "tcpdump"], default="auto", help="Packet capture backend. Default: auto.")
    p.add_argument("--filter", dest="bpf_filter", default="", help='Extra BPF filter to AND with IP/port filters. Empty for none.')
    p.add_argument("--py-filter", action="store_true", help="Apply a Python-level filter (avoids kernel BPF filter in scapy backend).")
    p.add_argument("--no-promisc", action="store_true", help="Disable promiscuous mode while sniffing.")

    # Analysis controls
    p.add_argument("--max-prefix", type=int, default=8, help="Maximum prefix length (bytes) to analyze. Default: 8")
    p.add_argument("--entropy-limit", type=float, default=2.0, help="Stop at the first prefix length where entropy exceeds this. Default: 2.0")
    p.add_argument("--min-pct", type=float, default=0.1, help="Minimum percentage to display a prefix line. Default: 0.1 (i.e., 0.1%%).")
    p.add_argument("--min-count", type=int, default=1, help="Minimum absolute count to display a prefix entry. Default: 1")
    p.add_argument("--coverage", type=float, default=0.0, help="Keep entries until cumulative coverage reaches this percent (<=0 disables).")
    p.add_argument("--show-others", action="store_true", help="Summarize filtered entries as an '(others)' line.")
    p.add_argument("--top-k", type=int, default=50, help="Limit lines per prefix length to top K (<=0 for unlimited). Default: 50")
    p.add_argument("--analysis-timeout", type=float, default=0.0, help="Hard cap (seconds) for the reporting section (<=0 disables).")
    p.add_argument("--no-entropy", action="store_true", help="Skip entropy computation during reporting.")

    # Misc
    p.add_argument("--progress-every", type=int, default=100000, help="Print progress every N packets (0 disables). Default: 100000")
    p.add_argument("--output", dest="output_file", help="Write results to this file instead of stdout.")
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

    try:
        iface = args.iface or select_default_iface()
    except Exception as e:
        print(f"Could not determine network interface: {e}", file=sys.stderr)
        sys.exit(2)

    analyze_live(
        iface=str(iface),
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
        backend=args.backend,
        top_k=args.top_k,
        no_entropy=args.no_entropy,
        analysis_timeout=args.analysis_timeout,
        min_count=args.min_count,
        coverage=args.coverage,
        show_others=args.show_others,
        ips=args.ip,
        src_ips=args.src_ip,
        dst_ips=args.dst_ip,
        ports=args.port,
        src_ports=args.src_port,
        dst_ports=args.dst_port,
        py_filter=args.py_filter,
    )


if __name__ == "__main__":
    main()

