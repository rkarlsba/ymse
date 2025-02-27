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
VM_IMG_DIR='/var/lib/libvirt/images'
RUN_COUNT=0

# Our loop
for vm in $@
do
    # 'Locals'
    vm_name="$vm.$DOMAIN"
    vm_file="$VM_IMG_DIR/$vm_name-root1.qcow2"

    # Sanity check
    if $( echo -n "$vm_name" | tr '[A-Z]' '[a-z]' | grep -Pvq '^[a-z0-9\.\-]+$' )
    then
        echo "Illegal hostname: $vm_name, exiting" >&2
        exit 1
    fi
    if ! $( virsh domstate $ORIGINAL > /dev/null 2>&1 )
    then
        echo "VM template $ORIGINAL doesn't seem to exist, exiting" >&2
        exit 2
    fi
    if $( virsh domstate $vm_name > /dev/null 2>&1 )
    then
        echo "VM $vm_name seems to exist, exiting" >&2
        exit 3
    fi
    if [ -f $vm_file ]
    then
        echo "New VM image $vm_file already exists, exiting" >&2
        exit 4
    fi

    # Don't a
    echo_cmd=''
    if [ $UID -eq 0 ]
    then
        echo "Cloning template VM $TEMPLATE_VM to new VM $vm_name"
    else
        if [ $RUN_COUNT -eq 0 ]
        then
            echo "Not root, posting the create command(s) below:"
        fi
        echo_cmd='echo'
    fi
    $echo_cmd virt-clone --original $ORIGINAL --name $vm_name --file $vm_file
    _exitcode=$?
    if [ $_exitcode != 0 ]
    then
        echo 'Something terrible happened - exiting' >&2
        exit $_exitcode
    else
        echo 'Done'
    fi
done
