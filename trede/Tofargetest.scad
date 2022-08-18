$fn = $preview ? 8 : 64;

rcor=2;
x=36;
y=20;
z=1;
tekst="Oppå";
ramme=z/2;

module plate() {
    linear_extrude(z) {
        hull() {
            translate([rcor,rcor]) circle(r=rcor);
            translate([x-rcor,rcor]) circle(r=rcor);
            translate([rcor,y-rcor]) circle(r=rcor);
            translate([x-rcor,y-rcor]) circle(r=rcor);
        }
    }
}

module ramme() {
    linear_extrude(ramme) {
        difference() {
            hull() {
                translate([rcor,rcor]) circle(r=rcor);
                translate([x-rcor,rcor]) circle(r=rcor);
                translate([rcor,y-rcor]) circle(r=rcor);
                translate([x-rcor,y-rcor]) circle(r=rcor);
            }
            hull() {
                translate([rcor+ramme,rcor+ramme]) circle(r=rcor);
                translate([x-rcor-ramme,rcor+ramme]) circle(r=rcor);
                translate([rcor+ramme,y-rcor-ramme]) circle(r=rcor);
                translate([x-rcor-ramme,y-rcor-ramme]) circle(r=rcor);
            }
        }
    }
}

module tekst() {
    translate([x/10,y*.3]) {
        linear_extrude(z) {
            text(tekst, size=8);
        }
    }
}

plate();
//tekst();
//ramme();