#!/bin/bash
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker

URL_FILE="urls.txt"
PKG_DIR="packages"

while IFS= read -r url
do
    url=$( echo "$url" | perl -pe 's/^\s*#.*//' )
    [[ "$url" = "" ]] && continue
    wget --directory-prefix=$PKG_DIR $url
done < $URL_FILE
