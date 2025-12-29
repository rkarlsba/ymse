#!/usr/bin/env python3

import numpy as np
from stl import mesh

input_filename = 'USB Micro Breakout lid.stl'
output_filename = 'USB Micro Breakout lid-new.stl'

# Load the STL file
your_mesh = mesh.Mesh.from_file('your_file.stl')

# Option 1: Shift so min coordinates are at [0,0,0]
min_coords = your_mesh.min_  # Shape (3,)
your_mesh.translate(-min_coords)

# Option 2: Center at center of gravity (often preferred)
volume, cog, inertia = your_mesh.get_mass_properties()
your_mesh.translate(-cog)

# Update mesh properties and save
your_mesh.update_min()
your_mesh.save('centered_file.stl')

