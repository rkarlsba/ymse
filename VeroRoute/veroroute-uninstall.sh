#!/bin/bash 

#
# To uninstall on Linux, type the following at the command line
#    sudo ./veroroute-uninstall.sh 
#

rm -f  "$pkgdir/usr/bin/veroroute"
rm -f  "$pkgdir/usr/share/applications/veroroute.desktop"
if find -- "$pkgdir/usr/share/applications/" -prune -type d -empty | grep -q .; then
   rmdir "$pkgdir/usr/share/applications/"
fi
rm -f  "$pkgdir/usr/share/pixmaps/veroroute.png"
if find -- "$pkgdir/usr/share/pixmaps/" -prune -type d -empty | grep -q .; then
   rmdir "$pkgdir/usr/share/pixmaps/"
fi
rm -f  "$pkgdir/usr/share/veroroute/veroroute.png"
rm -rf "$pkgdir/usr/share/veroroute/tutorials"
rmdir  "$pkgdir/usr/share/veroroute"
rm -rf "$pkgdir/usr/share/gEDA/sym/veroroute_*"
if find -- "$pkgdir/usr/share/gEDA/sym/" -prune -type d -empty | grep -q .; then
   rmdir "$pkgdir/usr/share/gEDA/sym/"
fi
rm -f  "$pkgdir/usr/share/gEDA/gafrc.d/veroroute-clib.scm"
if find -- "$pkgdir/usr/share/gEDA/gafrc.d/" -prune -type d -empty | grep -q .; then
   rmdir "$pkgdir/usr/share/gEDA/gafrc.d/"
fi


