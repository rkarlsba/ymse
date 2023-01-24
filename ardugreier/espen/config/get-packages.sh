#!/bin/bash
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker
#
# Written by Roy Sigurd Karlsbakk <roy@karlsbakk.net>
# Licensed under the Apache license version 2.0. Please
# see the LICENSE file for details.

URL_FILE="urls.txt"
PKG_DIR="sadf"

while IFS= read -r url
do
    url=$( echo "$url" | perl -pe 's/^\s*#.*//' )
    [[ "$url" = "" ]] && continue
    wget --directory-prefix=$PKG_DIR --quiet $url
done < $URL_FILE
