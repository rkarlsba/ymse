#!/usr/bin/env python3
# vim:ts=4:sw=4:sts=4:et:ai:si:tw=100:fdm=marker

# Written by Roy Sigurd Karlsbakk <roy@karlsbakk.net> in 2026 with a decent amount of help from
# various AIs
#
# Licensed under AGPL v3. See agpl-3.0.md for details.

import argparse
import os
import shutil
import sys
import json
import numpy as np
from stl import mesh


def mesh_info_dict(mesh_obj):
    """Returner mesh-data i et JSON-vennlig format."""
    min_coords = mesh_obj.min_
    max_coords = mesh_obj.max_
    dims = max_coords - min_coords
    volume, _, _ = mesh_obj.get_mass_properties()
    triangles = len(mesh_obj.data)
    return {
        "dimensions": [float(dims[0]), float(dims[1]), float(dims[2])],
        "min_corner": [float(min_coords[0]), float(min_coords[1]), float(min_coords[2])],
        "max_corner": [float(max_coords[0]), float(max_coords[1]), float(max_coords[2])],
        "volume": float(volume),
        "triangles": int(triangles),
        "vertices": int(triangles * 3),
    }


def print_info(mesh_obj, filename, label=""):
    """Skriv ut mesh-dimensjoner og egenskaper (tekstmodus)."""
    min_coords = mesh_obj.min_
    max_coords = mesh_obj.max_
    dims = max_coords - min_coords
    volume, _, _ = mesh_obj.get_mass_properties()
    triangles = len(mesh_obj.data)

    print(f"\n{label}STL Info: {filename}")
    print(f"  Dimensions: {dims[0]:.3f} x {dims[1]:.3f} x {dims[2]:.3f} mm")
    print(f"  Min corner: [{min_coords[0]:.3f}, {min_coords[1]:.3f}, {min_coords[2]:.3f}]")
    print(f"  Max corner: [{max_coords[0]:.3f}, {max_coords[1]:.3f}, {max_coords[2]:.3f}]")
    print(f"  Volume: {volume:.3f} mm³")
    print(f"  Triangles: {triangles}")
    print(f"  Vertices: {triangles * 3}")


def rotation_matrix_x(deg):
    rad = np.deg2rad(deg)
    c, s = np.cos(rad), np.sin(rad)
    return np.array([
        [1, 0,  0],
        [0, c, -s],
        [0, s,  c],
    ])


def rotation_matrix_y(deg):
    rad = np.deg2rad(deg)
    c, s = np.cos(rad), np.sin(rad)
    return np.array([
        [ c, 0, s],
        [ 0, 1, 0],
        [-s, 0, c],
    ])


def rotation_matrix_z(deg):
    rad = np.deg2rad(deg)
    c, s = np.cos(rad), np.sin(rad)
    return np.array([
        [c, -s, 0],
        [s,  c, 0],
        [0,  0, 1],
    ])


def main():
    parser = argparse.ArgumentParser(description='STL origin positioning tool')
    parser.add_argument('-i', '--input', required=True,
        help='Input STL filename')

    # Output mode: either write to a new file, or edit in place
    out_group = parser.add_mutually_exclusive_group()
    out_group.add_argument('-o', '--output',
        help='Output STL filename (omit for info only)')
    out_group.add_argument('--in-place', nargs='?', const='', metavar='SUFFIX',
        help='Edit input file in place; optionally create a backup with SUFFIX (e.g., ".bak")')

    # Positioning options
    pos_group = parser.add_mutually_exclusive_group()
    pos_group.add_argument('--bottom-left', action='store_true',
        help='Move min corner (bottom-left-front) to [0,0,0] (default)')
    pos_group.add_argument('--center', action='store_true',
        help='Center using center of gravity')
    pos_group.add_argument('--center-xy', action='store_true',
        help='Center X/Y, Z stays at bottom (sits on Z=0 plane)')
    pos_group.add_argument('--center-positive', action='store_true',
        help='Center object, then move so min corner is at [0,0,0]')

    # Rotation options (degrees)
    parser.add_argument('--rotate-x', type=float, default=0.0,
        help='Rotate around X axis (degrees)')
    parser.add_argument('--rotate-y', type=float, default=0.0,
        help='Rotate around Y axis (degrees)')
    parser.add_argument('--rotate-z', type=float, default=0.0,
        help='Rotate around Z axis (degrees)')

    # Verbose and JSON flags
    parser.add_argument('-v', '--verbose', action='store_true',
        help='Print detailed info (Before/After) when making changes')
    parser.add_argument('--json', action='store_true',
        help='Output all results as a single JSON object')

    args = parser.parse_args()

    # Helper to emit JSON and exit
    def emit_json_and_exit(obj, code=0):
        print(json.dumps(obj, ensure_ascii=False))
        sys.exit(code)

    # Interpret in-place and backup suffix
    in_place = args.in_place is not None
    backup_suffix = None
    if in_place and isinstance(args.in_place, str) and args.in_place != '':
        backup_suffix = args.in_place

    # Default positioning
    if not any([args.bottom_left, args.center, args.center_xy, args.center_positive]):
        args.bottom_left = True

    # Load mesh (error as JSON if needed)
    try:
        mesh_obj = mesh.Mesh.from_file(args.input)
    except Exception as e:
        if args.json:
            emit_json_and_exit({"ok": False, "error": f"Failed to load input file", "details": str(e), "input_path": args.input}, code=2)
        raise

    # Info-only mode (no changes)
    if not args.output and not in_place:
        if args.json:
            info = mesh_info_dict(mesh_obj)
            emit_json_and_exit({
                "ok": True,
                "status": "info_only",
                "message": f"STL file info only: {args.input}",
                "input_path": args.input,
                "in_place": False,
                "operations": {
                    "positioning": None,
                    "rotate_x": 0.0,
                    "rotate_y": 0.0,
                    "rotate_z": 0.0
                },
                "mesh_info": info
            })
        else:
            print(f"STL file info only: {args.input}")
            print_info(mesh_obj, args.input)
            return

    # If in-place, optionally create backup before modifying
    if in_place and backup_suffix:
        backup_path = args.input + backup_suffix
        if os.path.exists(backup_path):
            if args.json:
                emit_json_and_exit({
                    "ok": False,
                    "error": "Backup file already exists",
                    "backup_path": backup_path,
                    "input_path": args.input
                }, code=3)
            else:
                raise SystemExit(f"Backup file already exists: {backup_path}. Aborting to avoid overwrite.")
        try:
            shutil.copy2(args.input, backup_path)
        except Exception as e:
            if args.json:
                emit_json_and_exit({
                    "ok": False,
                    "error": "Failed to create backup",
                    "backup_path": backup_path,
                    "details": str(e),
                    "input_path": args.input
                }, code=4)
            else:
                raise
        if not args.json:
            print(f"Backup created: {backup_path}")
    else:
        backup_path = None

    if args.verbose and not args.json:
        print_info(mesh_obj, args.input, "Before: ")

    # ---- Rotation (around center of gravity) ----
    if args.rotate_x or args.rotate_y or args.rotate_z:
        _, cog, _ = mesh_obj.get_mass_properties()

        # Flytt tyngdepunkt til origo
        mesh_obj.translate(-cog)

        R = np.identity(3)
        if args.rotate_x:
            R = rotation_matrix_x(args.rotate_x) @ R
        if args.rotate_y:
            R = rotation_matrix_y(args.rotate_y) @ R
        if args.rotate_z:
            R = rotation_matrix_z(args.rotate_z) @ R

        # Bruk rotasjon
        mesh_obj.vectors = np.dot(mesh_obj.vectors, R.T)

        # Flytt tilbake
        mesh_obj.translate(cog)

        mesh_obj.update_min()
        mesh_obj.update_max()

    # ---- Positionering ----
    if args.bottom_left:
        offset = mesh_obj.min_

    elif args.center:
        _, cog, _ = mesh_obj.get_mass_properties()
        offset = cog

    elif args.center_xy:
        center_x = (mesh_obj.min_[0] + mesh_obj.max_[0]) / 2
        center_y = (mesh_obj.min_[1] + mesh_obj.max_[1]) / 2
        min_z = mesh_obj.min_[2]
        offset = np.array([center_x, center_y, min_z])

    elif args.center_positive:
        _, cog, _ = mesh_obj.get_mass_properties()
        mesh_obj.translate(-cog)
        mesh_obj.update_min()
        mesh_obj.update_max()
        offset = mesh_obj.min_

    mesh_obj.translate(-offset)
    mesh_obj.update_min()
    mesh_obj.update_max()

    # Bestem målsti og rapportering
    dest_path = args.input if in_place else args.output

    if args.verbose and not args.json:
        print_info(mesh_obj, dest_path, "After: ")

    # Lagre og rapporter
    try:
        mesh_obj.save(dest_path)
    except Exception as e:
        if args.json:
            emit_json_and_exit({
                "ok": False,
                "error": "Failed to save output",
                "details": str(e),
                "input_path": args.input,
                "output_path": dest_path,
                "in_place": in_place
            }, code=5)
        else:
            raise

    if args.json:
        result = {
            "ok": True,
            "status": "modified",
            "input_path": args.input,
            "output_path": dest_path,
            "in_place": in_place,
            "backup": {
                "created": backup_path is not None,
                "path": backup_path
            },
            "operations": {
                "positioning": (
                    "bottom_left" if args.bottom_left else
                    "center" if args.center else
                    "center_xy" if args.center_xy else
                    "center_positive" if args.center_positive else None
                ),
                "rotate_x": float(args.rotate_x),
                "rotate_y": float(args.rotate_y),
                "rotate_z": float(args.rotate_z),
            },
            "verbose": bool(args.verbose),
        }
        # Inkluder før/etter-info kun dersom -v er satt
        if args.verbose:
            # Re-konstruer før-info: les fra backup hvis finnes, ellers fra input før endring.
            # For enkelhet bruker vi kun 'after' når -v er satt, og inkluderer 'before' som None
            # hvis vi ikke har tatt vare på før-tilstand i minnet. Ønskes nøyaktig "before",
            # kan man laste på nytt fra backup_path eller input (før lagring). Her tar vi 'after' og
            # lar 'before' være None for å holde det enkelt og effektivt.
            result["before"] = None
            result["after"] = mesh_info_dict(mesh_obj)
        emit_json_and_exit(result)
    else:
        print(f"\nSaved to {dest_path}")


if __name__ == '__main__':
    main()

