#!/bin/bash 

#
# To uninstall on Linux, type the following at the command line
#    sudo ./veroroute-uninstall.sh 
#

rm -f  "$pkgdir/usr/local/bin/veroroute"
rm -f  "$pkgdir/usr/local/share/applications/veroroute.desktop"
if find -- "$pkgdir/usr/local/share/applications/" -prune -type d -empty | grep -q .; then
   rmdir "$pkgdir/usr/local/share/applications/"
fi
rm -f  "$pkgdir/usr/local/share/pixmaps/veroroute.png"
if find -- "$pkgdir/usr/local/share/pixmaps/" -prune -type d -empty | grep -q .; then
   rmdir "$pkgdir/usr/local/share/pixmaps/"
fi
rm -f  "$pkgdir/usr/local/share/veroroute/veroroute.png"
rm -rf "$pkgdir/usr/local/share/veroroute/tutorials"
rmdir  "$pkgdir/usr/local/share/veroroute"
rm -rf "$pkgdir/usr/local/share/gEDA/sym/veroroute_*"
if find -- "$pkgdir/usr/local/share/gEDA/sym/" -prune -type d -empty | grep -q .; then
   rmdir "$pkgdir/usr/local/share/gEDA/sym/"
fi
rm -f  "$pkgdir/usr/local/share/gEDA/gafrc.d/veroroute-clib.scm"
if find -- "$pkgdir/usr/local/share/gEDA/gafrc.d/" -prune -type d -empty | grep -q .; then
   rmdir "$pkgdir/usr/local/share/gEDA/gafrc.d/"
fi


