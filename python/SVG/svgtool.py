#!/usr/bin/env python3

"""
Normalize an SVG for OpenSCAD:
- Make +Y point up (Cartesian), origin at bottom-left.
- Bake the transform into path geometry (matrix(1 0 0 -1 0 H)).
- Write a clean viewBox: 0 0 W H.
- Avoid overwriting output unless --force is passed.

Usage:
    python normalize_svg.py input.svg output.svg
    python normalize_svg.py input.svg output.svg --force
"""

from __future__ import annotations

import sys
import argparse
from pathlib import Path
from typing import Tuple, Optional

from lxml import etree
from svgpathtools import svg2paths2, wsvg, Matrix


# --- Helpers ---------------------------------------------------------------

def _strip_unit(value: Optional[str]) -> Optional[float]:
    """Convert '100', '100px', '210mm', etc. to float user units.
    Note: If absolute units like mm/in are present, this function
    assumes they've already been mapped to user units by the authoring tool.
    If you need physical scaling, add unit conversion here.
    """
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

    # Fallback to width/height
    w = _strip_unit(svg_root.get("width"))
    h = _strip_unit(svg_root.get("height"))
    if w is None or h is None:
        raise ValueError(
            "SVG lacks a usable viewBox and width/height; "
            "cannot determine canvas size."
        )
    return w, h


def collect_style(attrs: dict) -> dict:
    """Extract a minimal set of style attributes from svgpathtools attrs."""
    out = {}
    # Common direct attributes
    for k in ("fill", "stroke", "stroke-width", "fill-rule", "stroke-linejoin",
              "stroke-linecap", "opacity", "fill-opacity", "stroke-opacity"):
        v = attrs.get(k)
        if v is not None:
            out[k] = v

    # Parse inline style="..."
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

def normalize_svg(infile: Path, outfile: Path) -> None:
    """Read SVG, flip Y (+ translate by H), write with clean viewBox."""
    # Parse DOM to get root and canvas size
    tree = etree.parse(str(infile))
    root = tree.getroot()

    if not root.tag.endswith("svg"):
        raise RuntimeError("Root element is not <svg>.")

    W, H = read_canvas_size(root)

    # Load paths and their attributes
    paths, attributes, svg_attrs = svg2paths2(str(infile))

    # Build Y-flip + translate(H) matrix: matrix(1 0 0 -1 0 H)
    flip = Matrix(1, 0, 0, -1, 0, H)

    # Transform paths; preserve basic styling
    new_paths = []
    new_attrs = []
    for p, a in zip(paths, attributes):
        tp = p.transformed(flip)
        new_paths.append(tp)
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
            "Normalize an SVG for OpenSCAD: bake a Cartesian (+Y up) coordinate "
            "system into path geometry and write a clean viewBox."
        )
    )
    p.add_argument("input", type=Path, help="Input SVG file")
    p.add_argument("output", type=Path, help="Output SVG file (will not overwrite unless --force)")
    p.add_argument(
        "-f", "--force",
        action="store_true",
        help="Overwrite output file if it already exists"
    )
    return p.parse_args(argv)


def main(argv: list[str]) -> int:
    args = parse_args(argv)

    infile: Path = args.input
    outfile: Path = args.output

    # Basic checks
    if not infile.exists():
        print(f"ERROR: Input not found: {infile}")
        return 1
    if infile.resolve() == outfile.resolve():
        print("ERROR: Input and output paths are the same. Refusing to overwrite input.")
        return 1
    if outfile.exists() and not args.force:
        print(f"ERROR: Output already exists: {outfile}\n"
              f"       Use --force to overwrite.")
        return 1

    try:
        # Ensure parent directory exists
        outfile.parent.mkdir(parents=True, exist_ok=True)
        normalize_svg(infile, outfile)
    except Exception as e:
        print(f"ERROR: {e}")
        return 2

    print(f"Normalized SVG written to: {outfile}")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))

