#!/bin/bash
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker

# System shite
now="$( date +%F\ %T )"

############################## <User changable> ##############################
vm="graylog.karlsbakk.net"
snapname="testsnap01"
snapdesc="Testsnap av $vm tatt $now"
debug=0
############################## </User changable> #############################

# funker ikke {{{
#   virsh snapshot-create-as \
#       --name $snapname \
#       --description $snapdesc \
#       --disk-only \
#       --live \
#       --domain $vm
# }}}

if [ $debug -gt 0 ]
then
    echo virsh snapshot-create-as \
        --name \"$snapname\" \
        --description \"$snapdesc\" \
        --disk-only \
        --live \
        --domain \"$vm\"
else
    virsh snapshot-create-as \
        --name "$snapname" \
        --description "$snapdesc" \
        --disk-only \
        --domain "$vm"
fi
