#!/usr/bin/env python3

"""
Normalize an SVG:
- Default: normalize only (retain SVG's +Y down), ideal for OpenSCAD which flips on import.
- Optional: --flip to bake a Cartesian (+Y up, origin bottom-left) transform into path geometry.
- Clean viewBox to 0 0 W H.
- Do not overwrite an existing output unless --force is given.

Usage:
    python normalize_svg.py input.svg output.svg
    python normalize_svg.py input.svg output.svg --flip
    python normalize_svg.py input.svg output.svg --force
"""

from __future__ import annotations

import sys
import argparse
from pathlib import Path
from typing import Tuple, Optional

from lxml import etree
from svgpathtools import svg2paths2, wsvg  # Path has .scaled() and .translated()

# --- Unit helpers ----------------------------------------------------------

def _strip_unit(value: Optional[str]) -> Optional[float]:
    """Convert '100', '100px', '210mm', etc. to float user units."""
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


# --- Core normalization ----------------------------------------------------

def normalize_svg(infile: Path, outfile: Path, flip: bool) -> None:
    """Read SVG, optionally flip Y, write with clean viewBox."""
    # Parse DOM to get root and canvas size
    tree = etree.parse(str(infile))
    root = tree.getroot()

    if not root.tag.endswith("svg"):
        raise RuntimeError("Root element is not <svg>.")

    W, H = read_canvas_size(root)

    # Load paths and attributes
    paths, attributes, svg_attrs = svg2paths2(str(infile))

    new_paths = []
    new_attrs = []

    for p, a in zip(paths, attributes):
        if flip:
            # Cartesian bake-in: scale Y by -1 about origin, then translate up by H
            ps = p.scaled(sx=1.0, sy=-1.0, origin=0+0j)
            pt = ps.translated(0 + H*1j)
            new_paths.append(pt)
        else:
            # OpenSCAD-friendly: leave geometry as-is; OpenSCAD will flip on import
            new_paths.append(p)

        new_attrs.append(collect_style(a))

    # Write out a clean SVG with normalized viewBox/size
    svg_out_attrs = {
        "viewBox": f"0 0 {W} {H}",
        "width": str(W),
        "height": str(H),
        "preserveAspectRatio": svg_attrs.get("preserveAspectRatio", "xMidYMid meet"),
    }

    wsvg(
        new_paths,
        attributes=new_attrs,
        svg_attributes=svg_out_attrs,
        filename=str(outfile)
    )


# --- CLI -------------------------------------------------------------------

def parse_args(argv: list[str]) -> argparse.Namespace:
    p = argparse.ArgumentParser(
        description=(
            "Normalize an SVG. "
            "Default keeps SVG's +Y down (good for OpenSCAD, which flips on import). "
            "Use --flip to bake a Cartesian (+Y up) transform."
        )
    )
    p.add_argument("input", type=Path, help="Input SVG file")
    p.add_argument("output", type=Path, help="Output SVG file (won't overwrite unless --force)")
    p.add_argument(
        "-f", "--force",
        action="store_true",
        help="Overwrite output file if it already exists"
    )
    p.add_argument(
        "--flip",
        action="store_true",
        help="Bake a Cartesian transform into the geometry (scale(1,-1) then translate(0,H))"
    )
    return p.parse_args(argv)


def main(argv: list[str]) -> int:
    args = parse_args(argv)

    infile: Path = args.input
    outfile: Path = args.output

    if not infile.exists():
        print(f"ERROR: Input not found: {infile}")
        return 1
    if infile.resolve() == outfile.resolve():
        print("ERROR: Input and output paths are the same. Refusing to overwrite input.")
        return 1
    if outfile.exists() and not args.force:
        print(f"ERROR: Output already exists: {outfile}\n       Use --force to overwrite.")
        return 1

    try:
        outfile.parent.mkdir(parents=True, exist_ok=True)
        normalize_svg(infile, outfile, flip=args.flip)
    except Exception as e:
        print(f"ERROR: {e}")
        return 2

    print(f"Normalized SVG written to: {outfile}")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))

