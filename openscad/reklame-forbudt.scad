// Pakkelapp

use <ymse.scad>
use <NoiseLib.scad>

$fn = $preview ? 12 : 64;

corner_rounding = 3;
line = 1;
hole_radius = 0;
label_thickness = 1;
text_thickness = 1.4;
fontsize=12.5;
fonttype="Apple Chancery:style=筆寫斜體";
//fonttype="Liberation Sans";
fontspacing=1;
bugaddition = $preview ? 0.1 : 0;

module ramme(xsize,ysize) {
    difference() {
        hull() {
            linear_extrude(label_thickness/3)
                roundedsquare([xsize,ysize], corner_rounding);
            translate([label_thickness,label_thickness,label_thickness/3])
                linear_extrude(label_thickness/3*5)
                    roundedsquare([xsize-label_thickness*2,ysize-label_thickness*2], corner_rounding);
        }
        translate([label_thickness+line,label_thickness+line,label_thickness])
            linear_extrude(label_thickness+bugaddition)
                roundedsquare([xsize-label_thickness*2-line*2,ysize-label_thickness*2-line*2], corner_rounding);
        if (hole_radius > 0) {
            translate([corner_rounding*1.5,ysize-corner_rounding*1.5,-bugaddition]) {
                cylinder(r=hole_radius,h=label_thickness+bugaddition*2);
            }
        }
    }
}

module tekst(tekst, storrelse, font=fonttype, spacing=1) {
    translate([6,7.5,label_thickness]) {
        linear_extrude(text_thickness) {
            text(tekst, size=storrelse, font=font, spacing=spacing);
        }
    }
}

ramme(144,23);
//tekst("Roy Sigurd Karlsbakk", 10, font=".New York:style=Regular");
//tekst("Reklame forbudt!", 10, font=".SF NS Rounded:style=Thin G1", spacing=1.13);
tekst("Reklame forbudt!", 10, font="Prime Minister of Canada:style=Regular", spacing=1.13);
