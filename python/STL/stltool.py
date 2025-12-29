#!/usr/bin/env python3

import argparse
import numpy as np
from stl import mesh

def print_info(mesh_obj, filename):
    """Print mesh dimensions and properties"""
    min_coords = mesh_obj.min_
    max_coords = mesh_obj.max_
    dims = max_coords - min_coords
    volume, _, _ = mesh_obj.get_mass_properties()
    triangles = len(mesh_obj.data)
    
    print(f"\nSTL Info: {filename}")
    print(f"  Dimensions: {dims[0]:.3f} x {dims[1]:.3f} x {dims[2]:.3f} mm")
    print(f"  Min corner: [{min_coords[0]:.3f}, {min_coords[1]:.3f}, {min_coords[2]:.3f}]")
    print(f"  Max corner: [{max_coords[0]:.3f}, {max_coords[1]:.3f}, {max_coords[2]:.3f}]")
    print(f"  Volume: {volume:.3f} mm³")
    print(f"  Triangles: {triangles}")
    print(f"  Vertices: {triangles * 3}")

def main():
    parser = argparse.ArgumentParser(description='STL origin tool')
    parser.add_argument('-i', '--input', required=True, 
        help='Input STL filename (required for both modes)')
    parser.add_argument('-o', '--output', 
        help='Output STL filename (required for processing)')
    parser.add_argument('-I', '--info', action='store_true', 
        help='Show STL info only (mutually exclusive with -o)')
    
    args = parser.parse_args()
    
    # Load mesh
    mesh_obj = mesh.Mesh.from_file(args.input)
    
    if args.info:
        if args.output:
            parser.error("-I/--info cannot be used with -o/--output")
        print_info(mesh_obj, args.input)
    else:
        if not args.output:
            parser.error("-o/--output required when not using -I/--info")
        # Process mode
        min_coords = mesh_obj.min_
        mesh_obj.translate(-min_coords)
        mesh_obj.update_min()
        mesh_obj.save(args.output)
        print(f"Moved bottom corner of {args.input} to [0,0,0]. Saved to {args.output}")

if __name__ == '__main__':
    main()

