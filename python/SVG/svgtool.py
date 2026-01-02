#!/usr/bin/env python3

# pip install svgpathtools lxml
from lxml import etree
from svgpathtools import svg2paths2, wsvg
import sys
from pathlib import Path

def read_size(svg_root):
    # Prefer viewBox if present
    vb = svg_root.get("viewBox")
    if vb:
        _, _, w, h = map(float, vb.split())
        return w, h
    # Fallback: width/height attributes (assume px/user units)
    def to_float(s):
        if s is None: return None
        for unit in ("mm","cm","in","pt","pc","px"):
            if s.endswith(unit):
                s = s[:-len(unit)]
                break
        return float(s)
    w = to_float(svg_root.get("width"))
    h = to_float(svg_root.get("height"))
    if w is None or h is None:
        raise ValueError("SVG lacks viewBox and width/height; cannot determine size.")
    return w, h

def normalize_svg(infile, outfile, cartesian_flip=True):
    # Load full DOM
    tree = etree.parse(str(infile))
    root = tree.getroot()

    # Namespaces
    ns = {"svg": "http://www.w3.org/2000/svg"}
    if not root.tag.endswith("svg"):
        raise RuntimeError("Root element is not <svg>.")

    W, H = read_size(root)

    # Parse all paths with svgpathtools (gets paths + attributes + svg attributes)
    paths, attrs, svg_attr = svg2paths2(str(infile))

    # Build transform matrix for flip: matrix(a b c d e f) = (1 0 0 -1 0 H)
    # svgpathtools uses complex transforms: z' = a*z + b, but exposes 'Matrix' helper via Path.d() export.
    # We'll apply by using complex scaling and translation directly to segments.
    # Method: scale y by -1, then translate y by H.
    def xy(c):  # complex -> tuple
        return (c.real, c.imag)

    from svgpathtools import Path, Line, CubicBezier, QuadraticBezier, Arc
    def transform_seg(seg):
        # For each segment point, apply (x, y) -> (x, -y + H)
        def T(c):
            return complex(c.real, -c.imag + H)
        if isinstance(seg, Line):
            return Line(T(seg.start), T(seg.end))
        elif isinstance(seg, CubicBezier):
            return CubicBezier(T(seg.start), T(seg.control1), T(seg.control2), T(seg.end))
        elif isinstance(seg, QuadraticBezier):
            return QuadraticBezier(T(seg.start), T(seg.control), T(seg.end))
        elif isinstance(seg, Arc):
            # Arc needs special handling: flipping Y negates sweep; easiest is to convert to cubic beziers
            # svgpathtools Path has 'as_cubic_curves()'
            # We'll let path-level conversion handle arcs later
            return seg
        else:
            return seg

    new_paths = []
    for p in paths:
        # Convert arcs to cubics to avoid arc parameter gymnastics under reflection
        pc = p.as_cubic_curves()
        new_paths.append(Path(*[transform_seg(s) for s in pc]))

    # Write out a plain SVG with baked paths and normalized viewBox
    # svgpathtools wsvg writes fresh SVG; we set viewBox and size explicitly
    svg_attrs = {
        "viewBox": f"0 0 {W} {H}",
        "width": str(W),
        "height": str(H)
    }
    wsvg(new_paths, attributes=[{} for _ in new_paths], svg_attributes=svg_attrs, filename=str(outfile))

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Usage: python normalize_svg.py input.svg output.svg")
        sys.exit(1)
    normalize_svg(Path(sys.argv[1]), Path(sys.argv[2]))

