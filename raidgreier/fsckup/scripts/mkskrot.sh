#!/bin/bash
# vim:ts=4:sw=4:sts=4:et:ai

if='/dev/urandom'
dir='skrot'
bs='256k'
blockcount=64
filecount=384

function die {
    print "$1" >&2
    exit 1
}

cd $dir
for (( i=0 ; i<$filecount ; i++ ))
do
    fn=$( printf "file%04d.skrot" $i )
    pn="$dir/$fn"
    # echo -n "$pn ..."
    # echo "dd if=$if of="$fn" bs=$bs count=$blockcount" ; exit
    dd if=$if of="$fn" bs=$bs count=$blockcount
    ec=$?
    echo
    
    if [ "$ec" -ne 0 ]
    then
        die "dd exited $ec" 
    fi
done
#     dd if=$if of="$pn" bs=$bs count=$blockcount > /dev/null 2>&1 || die "Feil: $?"
#     echo
# done
