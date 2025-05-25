#!/bin/bash
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker

ext=".dump"

for f in [g-z]*${ext}
do
    bn=$( basename ${f} ${ext} )
    grep -P 'ALTER TABLE .*? OWNER TO ' ${f} > ${bn}.owners
done
