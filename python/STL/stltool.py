#!/usr/bin/env python3

import argparse
import numpy as np
from stl import mesh

def print_info(mesh_obj, label=""):
    """Print mesh dimensions and properties"""
    min_coords = mesh_obj.min_
    max_coords = mesh_obj.max_
    dims = max_coords - min_coords
    volume, _, _ = mesh_obj.get_mass_properties()
    triangles = len(mesh_obj.data)
    
    print(f"\n{label}STL Info:")
    print(f"  Dimensions: {dims[0]:.3f} x {dims[1]:.3f} x {dims[2]:.3f} mm")
    print(f"  Min corner: [{min_coords[0]:.3f}, {min_coords[1]:.3f}, {min_coords[2]:.3f}]")
    print(f"  Max corner: [{max_coords[0]:.3f}, {max_coords[1]:.3f}, {max_coords[2]:.3f}]")
    print(f"  Volume: {volume:.3f} mm³")
    print(f"  Triangles: {triangles}")
    print(f"  Vertices: {triangles * 3}")

def main():
    parser = argparse.ArgumentParser(description='Move STL bottom corner to [0,0,0] for OpenSCAD')
    parser.add_argument('--input', '-i', help='Input STL filename')
    parser.add_argument('--output', '-o', help='Output STL filename')
    parser.add_argument('--info', '-I', action='store_true', help='Show STL dimensions/info')
    args = parser.parse_args()

    if not args.input:
        parser.error("Input file required for processing or info")

    # Load the STL file
    your_mesh = mesh.Mesh.from_file(args.input)
    
    # Show original info if requested
    if args.info:
        print_info(your_mesh, "Original ")

    # Move bottom corner to [0,0,0] if output specified
    if args.output:
        min_coords = your_mesh.min_
        your_mesh.translate(-min_coords)
        your_mesh.update_min()
        
        # Show processed info if requested
        if args.info:
            print_info(your_mesh, "Processed ")
        
        # Save
        your_mesh.save(args.output)
        print(f"\nSaved to {args.output}")
    elif args.info:
        print("\nNo output specified - info only")

if __name__ == '__main__':
    main()

