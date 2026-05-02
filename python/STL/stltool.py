#!/usr/bin/env python3
# vim:ts=4:sw=4:sts=4:et:ai:si:tw=100:fdm=marker

# Written by Roy Sigurd Karlsbakk <roy@karlsbakk.net> in 2026 with a decent amount of help from
# various AIs
#
# Licensed under AGPL v3. See agpl-3.0.md for details.


import argparse
import numpy as np
from stl import mesh


def print_info(mesh_obj, filename, label=""):
    """Print mesh dimensions and properties"""
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
    parser.add_argument('-o', '--output',
        help='Output STL filename (omit for info only)')

    # Positioning options
    group = parser.add_mutually_exclusive_group()
    group.add_argument('--bottom-left', action='store_true',
        help='Move min corner (bottom-left-front) to [0,0,0] (default)')
    group.add_argument('--center', action='store_true',
        help='Center using center of gravity')
    group.add_argument('--center-xy', action='store_true',
        help='Center X/Y, Z stays at bottom (sits on Z=0 plane)')
    group.add_argument('--center-positive', action='store_true',
        help='Center object, then move so min corner is at [0,0,0]')

    # Rotation options (degrees)
    parser.add_argument('--rotate-x', type=float, default=0.0,
        help='Rotate around X axis (degrees)')
    parser.add_argument('--rotate-y', type=float, default=0.0,
        help='Rotate around Y axis (degrees)')
    parser.add_argument('--rotate-z', type=float, default=0.0,
        help='Rotate around Z axis (degrees)')

    args = parser.parse_args()

    # Default positioning
    if not any([args.bottom_left, args.center, args.center_xy, args.center_positive]):
        args.bottom_left = True

    # Load mesh
    mesh_obj = mesh.Mesh.from_file(args.input)

    if not args.output:
        print_info(mesh_obj, args.input)
        return

    print_info(mesh_obj, args.input, "Before: ")

    # ---- Rotation (around center of gravity) ----
    if args.rotate_x or args.rotate_y or args.rotate_z:
        _, cog, _ = mesh_obj.get_mass_properties()

        # Move COG to origin
        mesh_obj.translate(-cog)

        R = np.identity(3)
        if args.rotate_x:
            R = rotation_matrix_x(args.rotate_x) @ R
        if args.rotate_y:
            R = rotation_matrix_y(args.rotate_y) @ R
        if args.rotate_z:
            R = rotation_matrix_z(args.rotate_z) @ R

        # Apply rotation
        mesh_obj.vectors = np.dot(mesh_obj.vectors, R.T)

        # Move back
        mesh_obj.translate(cog)

        mesh_obj.update_min()
        mesh_obj.update_max()

    # ---- Positioning ----
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

    print_info(mesh_obj, args.output, "After: ")

    mesh_obj.save(args.output)
    print(f"\nSaved to {args.output}")


if __name__ == '__main__':
    main()

