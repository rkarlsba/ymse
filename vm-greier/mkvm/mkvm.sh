#!/bin/bash
# vim:ts=4:sw=4:sts=4:et:ai:tw=80

# Clone a template MV to another with a new name. This script is very simple and
# relies on the source vm (aka template) and the destination vm residing on a
# single image file.
# 
# Note that 'globals' and 'locals' are mentioned here and differ with one being
# in caps and the other in lowercase. bash doesn't have globals or locals, but
# we'll just prepend that since it's convenient.
#
# Roy Sigurd Karlsbakk <roy@karlsbakk.net>, 2022
#
# Released under GPL v3

# 'Globals'
TEMPLATE_VM='debian-bullseye-mal'
DOMAIN='karlsbakk.net'

# Our loop
for vm in $@
do
    # Locals
    virt-clone --original debian-bullseye-mal.karlsbakk.net --name prat.karlsbak.net --file prat.karlsbak.net-root1.qcow2
    $_exitcode = $?
    if [ $_exitcode != 0 ]
    then
        echo 'Something terrible happened - exiting' >&2
        exit $_exitcode
    fi
done
