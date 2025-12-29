#!/usr/bin/env python3

import argparse
import numpy as np
from stl import mesh

def main():
    parser = argparse.ArgumentParser(description='Move STL bottom corner to [0,0,0] for OpenSCAD')
    parser.add_argument('--input', '-i', required=True, help='Input STL filename')
    parser.add_argument('--output', '-o', required=True, help='Output STL filename')
    args = parser.parse_args()

    # Load the STL file
    your_mesh = mesh.Mesh.from_file(args.input)

    # Get minimum coordinates (bottom-left-front corner)
    min_coords = your_mesh.min_  # Shape (3,) - [min_x, min_y, min_z]
    
    # Translate so min corner is at [0,0,0]
    your_mesh.translate(-min_coords)
    
    # Update cached min/max properties
    your_mesh.update_min()
    
    # Save the positioned mesh
    your_mesh.save(args.output)
    print(f"Moved bottom corner of {args.input} to [0,0,0]. Saved to {args.output}")
    print(f"New min coordinates: {your_mesh.min_}")

if __name__ == '__main__':
    main()

