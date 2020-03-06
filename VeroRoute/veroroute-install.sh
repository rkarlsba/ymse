#!/bin/bash 

#
# To install on Linux, type the following at the command line
#    sudo ./veroroute-install.sh 
#

install -Dm755 veroroute "$pkgdir/usr/local/bin/veroroute"
mkdir -p "$pkgdir/usr/local/share/applications/"
install -Dm644 "veroroute.desktop" "$pkgdir/usr/local/share/applications/"
mkdir -p "$pkgdir/usr/local/share/pixmaps/"
install -Dm644 veroroute.png "$pkgdir/usr/local/share/pixmaps/"
mkdir -p "$pkgdir/usr/local/share/veroroute"
install -Dm644 veroroute.png "$pkgdir/usr/local/share/veroroute/"
mkdir -p "$pkgdir/usr/local/share/veroroute/tutorials"
find tutorials -maxdepth 1 -type f -exec install -Dm644 "{}" "$pkgdir/usr/local/share/veroroute/tutorials" \;
mkdir -p "$pkgdir/usr/local/share/gEDA/sym"
(cd libraries/gEDA; find . -mindepth 2 -type f -exec install -Dm644 "{}" "$pkgdir/usr/local/share/gEDA/sym/{}" \;)
mkdir -p "$pkgdir/usr/local/share/gEDA/gafrc.d/"
install -Dm644 "libraries/gEDA/veroroute-clib.scm" "$pkgdir/usr/local/share/gEDA/gafrc.d/"

