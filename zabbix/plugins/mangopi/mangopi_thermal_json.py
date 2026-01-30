#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# vim:ts=4:sw=4:sts=4:et:ai:si:fdm=marker:tw=80

"""
Thanks to copilot for writing this

The human assistant, Roy Sigurd Karlsbakk <roy@karslbakk.net>, merely looked
through this and checked that it worked and added some vim shite to make it
easier to handle later.

mangopi_thermal_json.py
Read Linux thermal zones and output JSON for Zabbix.
- Works on ARM boards exposing /sys/class/thermal/thermal_zone*/
- Converts millidegrees to Celsius (float, 1 decimal)
- Produces stable keys by normalizing zone "type" values

Examples:
  ./mangopi_thermal_json.py
  ./mangopi_thermal_json.py --pretty
  ./mangopi_thermal_json.py --include cpu_thermal,gpu_thermal
  ./mangopi_thermal_json.py --lld             # Zabbix LLD discovery payload
"""

import argparse
import glob
import json
import os
import re
import sys
from typing import Dict, List, Tuple

thermal_glob = "/sys/class/thermal/thermal_zone*"

def normalize_key(s: str) -> str:
    """Lowercase and keep only [a-z0-9_], collapse others into underscores."""
    s = s.strip().lower()
    s = re.sub(r"[^a-z0-9_]+", "_", s)
    s = re.sub(r"_+", "_", s)
    return s.strip("_") or "unknown"

def read_zone(zpath: str) -> Tuple[str, float]:
    """Return (key, temp_c) for a thermal zone path. Raises on hard failures."""
    type_path = os.path.join(zpath, "type")
    temp_path = os.path.join(zpath, "temp")

    try:
        with open(type_path, "r", encoding="utf-8") as f:
            ztype = f.read().strip()
    except Exception:
        ztype = os.path.basename(zpath)  # fallback to zone directory name

    key = normalize_key(ztype)

    with open(temp_path, "r", encoding="utf-8") as f:
        raw = f.read().strip()

    # Many kernels expose millidegrees, but some expose plain degrees.
    # Heuristic: values > 200 likely millidegrees (e.g. 59800 → 59.8°C).
    try:
        val = float(raw)
    except ValueError:
        # Try to remove any non-digits if present (rare)
        raw2 = re.sub(r"[^0-9\.\-]", "", raw)
        val = float(raw2) if raw2 else float("nan")

    temp_c = val / 1000.0 if val > 200 else val
    # Round to 1 decimal for tidy output
    temp_c = round(temp_c, 1)
    return key, temp_c

def collect(include: List[str] = None, exclude: List[str] = None) -> Dict[str, float]:
    """Collect zone readings into a dict."""
    include = [normalize_key(x) for x in (include or [])]
    exclude = [normalize_key(x) for x in (exclude or [])]

    data: Dict[str, float] = {}
    for z in sorted(glob.glob(thermal_glob)):
        try:
            key, temp_c = read_zone(z)
        except FileNotFoundError:
            # Zone disappeared between glob and open; skip quietly
            continue
        except Exception:
            # Any other I/O problem → skip this zone
            continue

        if include and key not in include:
            continue
        if exclude and key in exclude:
            continue

        # If duplicate key appears, suffix with index to keep JSON valid and deterministic
        final_key = key
        idx = 2
        while final_key in data:
            final_key = f"{key}_{idx}"
            idx += 1

        data[final_key] = temp_c
    return data

def lld_payload(include: List[str] = None, exclude: List[str] = None) -> Dict[str, List[Dict[str, str]]]:
    """
    Produce Zabbix Low-Level Discovery JSON:
    {
      "data": [
        {"{#ZONETYPE}": "cpu_thermal", "{#ITEMKEY}": "cpu_thermal"},
        ...
      ]
    }
    """
    data = collect(include, exclude)
    return {
        "data": [
            {
                "{#ZONETYPE}": k,
                "{#ITEMKEY}": k
            }
            for k in data.keys()
        ]
    }

def main() -> int:
    ap = argparse.ArgumentParser(description="Output MangoPi thermal readings in JSON")
    ap.add_argument("--pretty", action="store_true", help="Pretty-print JSON")
    ap.add_argument("--include", help="Comma-separated list of zone types to include (normalized names)")
    ap.add_argument("--exclude", help="Comma-separated list of zone types to exclude (normalized names)")
    ap.add_argument("--lld", action="store_true", help="Emit Zabbix LLD discovery JSON instead of values")
    args = ap.parse_args()

    include = args.include.split(",") if args.include else None
    exclude = args.exclude.split(",") if args.exclude else None

    try:
        if args.lld:
            payload = lld_payload(include, exclude)
        else:
            payload = collect(include, exclude)
    except Exception as e:
        # Make failures visible to Zabbix
        print(json.dumps({"error": str(e)}))
        return 1

    if args.pretty:
        print(json.dumps(payload, indent=2, sort_keys=True))
    else:
        print(json.dumps(payload, separators=(",", ":"), sort_keys=True))

    return 0

if __name__ == "__main__":
    sys.exit(main())

