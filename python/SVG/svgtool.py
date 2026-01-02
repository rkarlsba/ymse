#!/usr/bin/env python3

"""
Normalize/Tighten an SVG for OpenSCAD or general use.

Defaults:
- Keep SVG's native +Y down orientation (best for OpenSCAD; it flips on import).
- Write a clean viewBox.

Options:
- --tight : Crop viewBox to the drawing's tight bounding box and translate paths so minX/minY => 0,0.
- --flip  : Bake Cartesian (+Y up) into geometry (scale(1,-1) then translate(0,H)).
- --force : Allow overwriting output if it exists.

Examples:
    python normalize_svg.py in.svg out.svg --tight
    python normalize_svg.py in.svg out.svg --flip --tight
    python normalize_svg.py in.svg out.svg --force
"""

from __future__ import annotations

import sys
import argparse
from pathlib import Path
from typing import Tuple, Optional

from lxml import etree
from svgpathtools import svg2paths2, wsvg  # Path has .scaled(), .translated()

# --- Unit helpers ----------------------------------------------------------

def _strip_unit(value: Optional[str]) -> Optional[float]:
    """Convert '100', '100px', '210mm', etc., to float user units."""
    if value is None:
        return None
    s = value.strip()
    for unit in ("px", "mm", "cm", "in", "pt", "pc"):
        if s.endswith(unit):
            s = s[:-len(unit)]
            break
    try:
        return float(s)
    except ValueError:
        return None


def read_canvas_size(svg_root: etree._Element) -> Tuple[float, float]:
    """Return (W, H) from viewBox or width/height; raise if missing."""
    vb = svg_root.get("viewBox")
    if vb:
        parts = vb.replace(",", " ").split()
        if len(parts) == 4:
            _, _, w, h = map(float, parts)
            return w, h

    w = _strip_unit(svg_root.get("width"))
    h = _strip_unit(svg_root.get("height"))
    if w is None or h is None:
        raise ValueError(
            "SVG lacks a usable viewBox and width/height; cannot determine canvas size."
        )
    return w, h


def collect_style(attrs: dict) -> dict:
    """Extract a minimal set of style attributes to preserve in output."""
    out = {}
    for k in ("fill", "stroke", "stroke-width", "fill-rule", "stroke-linejoin",
              "stroke-linecap", "opacity", "fill-opacity", "stroke-opacity"):
        v = attrs.get(k)
        if v is not None:
            out[k] = v

    style = attrs.get("style")
    if style:
        for item in style.split(";"):
            item = item.strip()
            if not item:
                continue
            if ":" in item:
                k, v = item.split(":", 1)
                k = k.strip(); v = v.strip()
                if k in ("fill", "stroke", "stroke-width", "fill-rule",
                         "stroke-linejoin", "stroke-linecap",
                         "opacity", "fill-opacity", "stroke-opacity"):
                    out.setdefault(k, v)
    return out


# --- Bounding box utilities ------------------------------------------------

def path_bbox(p) -> Tuple[float, float, float, float]:
    """
    Return (minx, miny, maxx, maxy) for a Path.
    svgpathtools provides bounding-box methods on path/segments; we rely on them. [3](https://baszerr.eu/doku.php?id=blog:2022:04:07:2022-04-07_-_importing_svg_in_openscad)
    """
    # Path has .bbox() in recent versions. Fallback: union of segment bboxes.
    try:
        minx, miny, maxx, maxy = p.bbox()
        return minx, miny, maxx, maxy
    except Exception:
        # Manual union over segments
        minx = miny = float("inf")
        maxx = maxy = float("-inf")
        for s in p:
            try:
                sxmin, symin, sxmax, symax = s.bbox()
            except Exception:
                # As a very conservative fallback, include endpoints
                xs = [s.start.real, s.end.real]
                ys = [s.start.imag, s.end.imag]
                sxmin, sxmax = min(xs), max(xs)
                symin, symax = min(ys), max(ys)
            minx = min(minx, sxmin)
            miny = min(miny, symin)
            maxx = max(maxx, sxmax)
            maxy = max(maxy, symax)
        return minx, miny, maxx, maxy


def union_bbox(paths) -> Tuple[float, float, float, float]:
    """Union bbox across all paths."""
    if not paths:
        return 0.0, 0.0, 0.0, 0.0
    minx = miny = float("inf")
    maxx = maxy = float("-inf")
    for p in paths:
        pxmin, pymin, pxmax, pymax = path_bbox(p)
        minx = min(minx, pxmin)
        miny = min(miny, pymin)
        maxx = max(maxx, pxmax)
        maxy = max(maxy, pymax)
    return minx, miny, maxx, maxy


# --- Core normalization ----------------------------------------------------

def normalize_svg(infile: Path, outfile: Path, flip: bool, tight: bool) -> None:
    """Read SVG, optionally flip Y, optionally tighten to bbox, write with clean viewBox."""
    # Parse DOM to get root and canvas size
    tree = etree.parse(str(infile))
    root = tree.getroot()

    if not root.tag.endswith("svg"):
        raise RuntimeError("Root element is not <svg>.")

    W, H = read_canvas_size(root)

    # Load paths and attributes
    paths, attributes, svg_attrs = svg2paths2(str(infile))

    # Step 1: Optional flip (Cartesian bake-in).

