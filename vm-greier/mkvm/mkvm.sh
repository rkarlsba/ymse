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
# Released under GPL v3 - see LICENSE.md for details

# 'Globals'
TEMPLATE_VM='debian-bullseye-mal'
DOMAIN='karlsbakk.net'
ORIGINAL="$TEMPLATE_VM.$DOMAIN"

# Our loop
for vm in $@
do
    # 'Locals'
    vm_name="$vm.$DOMAIN"
    vm_file="$vm_name-root1.qcow2"

    # Sanity check
    if $( echo "$vm_name" | perl -ne 'exit 1 unless (/^[a-z0-9\-\.]+/);' )
    then
        echo "Illegal hostname: $vm_name, exiting"
        exit 1
    fi
    if $( virsh domstate $ORIGINAL > /dev/null 2>&1 )
    then
        echo "VM template $ORIGINAL seems to exist, exiting"
        exit 2
    fi
    if $( virsh domstate $vm_name > /dev/null 2>&1 )
    then
        echo "VM $vm_name seems to exist, exiting"
        exit 3
    fi

    echo Cloning template VM $TEMPLATE_VM to new VM $vm_name

    virt-clone --original $ORIGINAL --name $vm_name --file $vm_file
    if [ $_exitcode != 0 ]
    then
        echo 'Something terrible happened - exiting' >&2
        exit $_exitcode
    else
        echo 'Done'
    fi
done
