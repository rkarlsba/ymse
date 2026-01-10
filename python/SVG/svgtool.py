#!/usr/bin/env python3

"""
Normalize/Tighten an SVG.

Defaults:
- Keep SVG's native +Y down (best for OpenSCAD; it flips on import).
- Write a clean viewBox.

Options:
- --tight : Crop viewBox to the drawing's tight bounding box and translate paths so minX/minY => 0,0.
- --flip  : Bake Cartesian (+Y up) into geometry (scale(1,-1) then translate(0,H)).
- --force : Allow overwriting output.
- --verbose : Print details (sizes, path counts, bbox).

Usage:
    python svgtool.py in.svg out.svg --tight
    python svgtool.py in.svg out.svg --flip --tight
    python svgtool.py in.svg out.svg --force --verbose
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
    vb = svg_root.get("viewBox")
    if vb:
        parts = vb.replace(",", " ").split()
        if len(parts) == 4:
            _, _, w, h = map(float, parts)
            return w, h

    w = _strip_unit(svg_root.get("width"))
    h = _strip_unit(svg_root.get("height"))
    if w is None or h is None:
        raise ValueError("SVG lacks viewBox and width/height; cannot determine canvas size.")
    return w, h


def collect_style(attrs: dict) -> dict:
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
    try:
        return p.bbox()  # minx, miny, maxx, maxy
    except Exception:
        minx = miny = float("inf")
        maxx = maxy = float("-inf")
        for s in p:
            try:
                sxmin, symin, sxmax, symax = s.bbox()
            except Exception:
                xs = [s.start.real, s.end.real]
                ys = [s.start.imag, s.end.imag]
                sxmin, sxmax = min(xs), max(xs)
                symin, symax = min(ys), max(ys)
            minx = min(minx, sxmin); miny = min(miny, symin)
            maxx = max(maxx, sxmax); maxy = max(maxy, symax)
        return minx, miny, maxx, maxy


def union_bbox(paths) -> Tuple[float, float, float, float]:
    if not paths:
        return 0.0, 0.0, 0.0, 0.0
    minx = miny = float("inf")
    maxx = maxy = float("-inf")
    for p in paths:
        pxmin, pymin, pxmax, pymax = path_bbox(p)
        minx = min(minx, pxmin); miny = min(miny, pymin)
        maxx = max(maxx, pxmax); maxy = max(maxy, pymax)
    return minx, miny, maxx, maxy


# --- Core normalization ----------------------------------------------------

def normalize_svg(infile: Path, outfile: Path, flip: bool, tight: bool, verbose: bool) -> None:
    # Parse DOM to get root and canvas size
    tree = etree.parse(str(infile))
    root = tree.getroot()
    if not root.tag.endswith("svg"):
        raise RuntimeError("Root element is not <svg>.")

    W, H = read_canvas_size(root)
    if verbose:
        print(f"[info] canvas W×H = {W}×{H}")

    # Load paths and attributes
    paths, attributes, svg_attrs = svg2paths2(str(infile))
    if verbose:
        print(f"[info] parsed {len(paths)} path(s)")

    # Step 1: Optional flip (Cartesian bake-in).
    working_paths = []
    for p in paths:
        if flip:
            ps = p.scaled(sx=1.0, sy=-1.0, origin=0+0j)
            pt = ps.translated(0 + H*1j)
            working_paths.append(pt)
        else:
            working_paths.append(p)

    # Step 2: Optional tightening to artwork bbox.
    if tight:
        bxmin, bymin, bxmax, bymax = union_bbox(working_paths)
        tight_w = max(0.0, bxmax - bxmin)
        tight_h = max(0.0, bymax - bymin)
        if verbose:
            print(f"[info] bbox min=({bxmin:.3f},{bymin:.3f}) max=({bxmax:.3f},{bymax:.3f}) size={tight_w:.3f}×{tight_h:.3f}")

        # If there are no paths, just tighten the viewBox but keep geometry (nothing to translate)
        if working_paths:
            working_paths = [p.translated((-bxmin) + (-bymin)*1j) for p in working_paths]
        out_viewbox = (0.0, 0.0, tight_w, tight_h)
        out_width, out_height = tight_w, tight_h
    else:
        out_viewbox = (0.0, 0.0, W, H)
        out_width, out_height = W, H

    # Prepare attributes per path (preserve simple styling)
    new_attrs = [collect_style(a) for a in attributes]

    # Write out via wsvg
    svg_out_attrs = {
        "viewBox": f"{out_viewbox[0]} {out_viewbox[1]} {out_viewbox[2]} {out_viewbox[3]}",
        "width": str(out_width),
        "height": str(out_height),
        "preserveAspectRatio": svg_attrs.get("preserveAspectRatio", "xMidYMid meet"),
    }
    wsvg(working_paths, attributes=new_attrs, svg_attributes=svg_out_attrs, filename=str(outfile))

    # Safety: if wsvg didn't create the file, write via lxml as a fallback
    if not outfile.exists():
        if verbose:
            print("[warn] wsvg did not produce output; writing minimal SVG via lxml.")
        # Build a minimal SVG with just viewBox/size (no geometry if paths==[])
        NS = "http://www.w3.org/2000/svg"
        svg = etree.Element("{%s}svg" % NS, nsmap={None: NS})
        svg.set("viewBox", svg_out_attrs["viewBox"])
        svg.set("width",   svg_out_attrs["width"])
        svg.set("height",  svg_out_attrs["height"])
        svg.set("preserveAspectRatio", svg_out_attrs["preserveAspectRatio"])
        # If desired, copy original children here. For now, just save the adjusted root.
        etree.ElementTree(svg).write(str(outfile), encoding="utf-8", xml_declaration=True)

    # Final check
    if not outfile.exists():
        raise RuntimeError("Output file was not created.")


# --- CLI -------------------------------------------------------------------

def parse_args(argv: list[str]) -> argparse.Namespace:
    p = argparse.ArgumentParser(
        description=("Normalize an SVG. Default keeps SVG's +Y down (OpenSCAD flips on import). "
                     "Use --flip to bake Cartesian (+Y up). Use --tight to crop to the drawing and remove margins.")
    )
    p.add_argument("input", type=Path, help="Input SVG file")
    p.add_argument("output", type=Path, help="Output SVG file (won't overwrite unless --force)")
    p.add_argument("-f", "--force", action="store_true", help="Overwrite output if it already exists")
    p.add_argument("--flip", action="store_true",
                   help="Bake Cartesian transform into geometry (scale(1,-1) then translate(0,H))")
    p.add_argument("--tight", action="store_true",
                   help="Crop viewBox to tight bbox and translate paths so minX/minY => 0,0")
    p.add_argument("--verbose", action="store_true", help="Print details while processing")
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
        normalize_svg(infile, outfile, flip=args.flip, tight=args.tight, verbose=args.verbose)
    except Exception as e:
        print(f"ERROR: {e}")
        return 2

    print(f"Normalized SVG written to: {outfile}")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))

