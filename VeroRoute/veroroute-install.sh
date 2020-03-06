#!/bin/bash 

#
# To install on Linux, type the following at the command line
#    sudo ./veroroute-install.sh 
#

install -Dm755 veroroute "$pkgdir/usr/bin/veroroute"
mkdir -p "$pkgdir/usr/share/applications/"
install -Dm644 "veroroute.desktop" "$pkgdir/usr/share/applications/"
mkdir -p "$pkgdir/usr/share/pixmaps/"
install -Dm644 veroroute.png "$pkgdir/usr/share/pixmaps/"
mkdir -p "$pkgdir/usr/share/veroroute"
install -Dm644 veroroute.png "$pkgdir/usr/share/veroroute/"
mkdir -p "$pkgdir/usr/share/veroroute/tutorials"
find tutorials -maxdepth 1 -type f -exec install -Dm644 "{}" "$pkgdir/usr/share/veroroute/tutorials" \;
mkdir -p "$pkgdir/usr/share/gEDA/sym"
(cd libraries/gEDA; find . -mindepth 2 -type f -exec install -Dm644 "{}" "$pkgdir/usr/share/gEDA/sym/{}" \;)
mkdir -p "$pkgdir/usr/share/gEDA/gafrc.d/"
install -Dm644 "libraries/gEDA/veroroute-clib.scm" "$pkgdir/usr/share/gEDA/gafrc.d/"

