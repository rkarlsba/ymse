#!/usr/bin/env python3

#reimplementation of basic gridfinity grid design by Zack Freedman
#designed in CadQuery and CQ-Editor both from github out on 2022-04-13

import cadquery as cq
import math
from cadquery import exporters

if 'show_object' not in globals():
    def show_object(*args, **kwargs):
        pass

output_dir = 'stl'

#adjust these two variables to change the number of grid positions
x_grid_number = 5 #CHANGEME!
y_grid_number = 7 #CHANGEME!

################################
# shouldn't need to adjust anything below here
box_wd = 42 #mm
box_sub_wd = 42.71 #mm, x,y width of box to subtract
c_rad = 4 #mm
grid_ht = 5 #mm, overall grid height
#grid_subht = 4.65

b_chm_ht = 0.985/math.sqrt(2) #mm, base chamfer height
strt_ht = 1.8 #mm, straight wall height
t_chm_ht = grid_ht - b_chm_ht - strt_ht #top chamfer height

#create 2D sketch with rounded corners
s3a = (
   cq.Sketch()
   .rect(box_sub_wd,box_sub_wd)
   .vertices().fillet(c_rad)
   )

#take s3a sketch and create the tool that is later used to subtract grid positions
f2 = (
   cq.Workplane("XY")
   .placeSketch(s3a)
   .extrude(t_chm_ht*math.sqrt(2),taper=45)
   .faces(">Z").wires().toPending()
   .extrude(strt_ht)
   .faces(">Z").wires().toPending()
   .extrude(b_chm_ht*math.sqrt(2),taper=45)
   .rotate((0, 0, 0), (1, 0, 0), 180)
   .translate((box_wd/2,box_wd/2,grid_ht))
)

#pts is the locations of each grid position
pts = [(x*box_wd, y*box_wd) for x in range(0,x_grid_number) for y in range(0,y_grid_number)]

#preunion all the individual tools for later cutting
f3 = (
   cq.Workplane("XY")
   .pushPoints(pts)
   .eachpoint(lambda loc: f2.val().moved(loc), combine="a", clean=True)
)


#create the big baseplate, and finally subtract the grid positions
f5 = (
   cq.Workplane("XY")
   .box(x_grid_number*box_wd,y_grid_number*box_wd,grid_ht-.001)
   .edges("|Z").fillet(c_rad)
   .translate((x_grid_number*box_wd/2,y_grid_number*box_wd/2,grid_ht/2))
   .faces(">Z")
   .cut(f3)
   )

show_object(f5,options={"alpha":0.10, "color": (65, 94, 55)})
#show_object(f2,options={"alpha":0.10, "color": (165, 94, 55)})

filename = f"{output_dir}/grid " + str(x_grid_number) + "x" + str(y_grid_number) + ".stl"
print(filename)
exporters.export(f5, filename, tolerance=0.99, angularTolerance=0.5)
