#!/bin/bash
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker

URL_FILE="urls.txt"

while IFS= read -r url
do
    url=$( echo "$url" | perl -pe 's/^\s*#.*//' )
    [[ "$url" = "" ]] && continue
    echo $url
done < $URL_FILE
