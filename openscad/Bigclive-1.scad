/*
 * Original code by BigClive
 * Code cleanup by Roy Sigurd Karlsbakk <roy@karlsbakk.net>
 * After all - the three cargo rules in coding is modularity, readability and readability!
 */

// MKII adaptor for 120mm fan
$fn = $preview ? 16 : 128;

module fan_adaptor(depth=40, hole=4) {
    // You can adjust the two variables below
    // depth = 40;           //depth of air chamber (40) - set as default in module()
    // hole  = 4;            //fan screw holes (4) - set as default in module()

    difference() {
        union() {
            // Baseplate
            hull() {
                for (i=[0:90:330])rotate([0, 0, i]) {
                    translate([74,0,0]) {
                        cylinder(h=1.5, d1=15, d2=15);
                    }
                }
            }
            // Main outer cylinder
            cylinder(h=depth, d1=120, d2=120);
        }
        // Internal cylinder
        translate([0,0,-1]) {
            cylinder(h=depth+2,d1=118,d2=118);
        }
        // Screw holes
        for (i=[0:90:330]) {
            rotate([0, 0, i]) {
                translate([74.5,0,-1]) {
                    cylinder(h=4, d1=hole, d2=hole);
                }
            }
        }
    }
}

module platen_for_filter_paper(depth=10, platen=1.5) {
    //MK3 platen for filter paper on 120mm fan
    //You can adjust the variables below
    // depth  = 10;       // Depth of lip and fins (10) - set as default in module()
    // platen = 1.5;      // Thickness of platen (1.5) - set as default in module()
    
    // Don't adjust anything below here
    hole = platen + 2;
    difference() {
        union() {
            // Platen
            cylinder(h=platen, d1=140, d2=140);
            // Main outer cylinder
            cylinder(h=depth+platen, d1=118, d2=118);
        }
        // Internal cylinder
        translate([0,0,platen]) {
            cylinder(h=depth+platen,d1=116,d2=116);
        }
        // Filter hole pattern
        for (i=[0:45:330]) {
            rotate([0, 0, i]) {
                translate([35,0,-1]) {
                    cylinder(h=hole, d1=20, d2=20);
                }
                translate([16,0,-1]) {
                    cylinder(h=hole, d1=10, d2=10);
                }
            }
        }
        for (i=[22.5:45:350]) {
            rotate([0, 0, i]) {
                translate([24,0,-1]) {
                    cylinder(h=hole, d1=10, d2=10);
                }
                translate([41,0,-1]) {
                    cylinder(h=hole, d1=10, d2=10);
                }
                translate([32.5,0,-1]) {
                    cylinder(h=hole, d1=5, d2=5);
                }
            }
        }
        translate([0,0,-1]) {
            cylinder(h=hole, d1=20, d2=20);
        }
    }
    // Anti turbulence fins
    difference() {
        for (i=[0:45:330]) {
            rotate([0, 0, i]) {
                translate([46,-0.5,0]) {
                    cube([12,1,depth+platen]);
                }
            }
        }
    }
}


// The third script is for the feet.  You will need to print four of them.

//MKII leg for 120mm fan
module fan_leg(depth = 50, hole = 4) {
    // You can adjust the two variables below
    // depth=50;       // Height of leg - set as default in module()
    // hole=4;         // Screw holes - set as default in module()
    difference() {
        union() {
            //Main outer cylinder
            cylinder(h=depth, d1=12, d2=12);
        }
        //internal cylinder
        translate([0,0,2]) {
            cylinder(h=depth+2,d1=10,d2=8);
        }
        translate([0,0,-1]) {
            cylinder(h=4,d1=hole,d2=hole);
        }
        translate([0,0,depth-1]) {
            cylinder(h=2,d1=10,d2=10);
        }
    }
}    

// Weighted ring for paper
module weighted_ring(depth=2, inner=100, outer=140) {
    //The variables below can be adjusted if desired
    // depth=2;       // Depth (weight) of ring - set as default in module()
    //inner=100;      // Inner hole diameter of ring - set as default in module()
    //outer=140;      // Outer diameter of ring - set as default in module()
    difference() {
        // Main disk
        cylinder(h=depth, d1=outer, d2=outer);
        // hole in middle
        translate([0,0,-1]) {
            cylinder(h=depth+2,d1=inner,d2=inner);
        }
    }
}

fan_adaptor();
translate([100,100,0]) {
    platen_for_filter_paper();
}
for (i=[0:15:45]) {
    translate([50+i,-50+i,0]) {
        fan_leg();
    }
}
translate([-100,-100,0]) {
    weighted_ring();
}