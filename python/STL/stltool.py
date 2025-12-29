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
    subparsers = parser.add_subparsers(dest='mode', required=True)
    
    # Processing mode: input + required output
    process_parser = subparsers.add_parser('process', 
        help='Move STL bottom corner to [0,0,0]')
    process_parser.add_argument('-i', '--input', required=True, 
        help='Input STL filename')
    process_parser.add_argument('-o', '--output', required=True, 
        help='Output STL filename')
    
    # Info mode: single filename
    info_parser = subparsers.add_parser('info', aliases=['-I', '--info'],
        help='Show STL dimensions/info')
    info_parser.add_argument('filename', nargs=1, 
        help='STL filename to analyze')
    
    args = parser.parse_args()
    
    if args.mode == 'process':
        your_mesh = mesh.Mesh.from_file(args.input)
        min_coords = your_mesh.min_
        your_mesh.translate(-min_coords)
        your_mesh.update_min()
        your_mesh.save(args.output)
        print(f"Moved bottom corner of {args.input} to [0,0,0]. Saved to {args.output}")
        
    elif args.mode in ['info', '-I', '--info']:
        filename = args.filename[0]
        your_mesh = mesh.Mesh.from_file(filename)
        print_info(your_mesh, filename)

if __name__ == '__main__':
    main()

