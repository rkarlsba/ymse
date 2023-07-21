#!/bin/bash

homedir='/data/ymse/rtl_433'

#protocols='DEFAULT'
protocols='ALL'
#protocols='{1..10} {12,15}'

verbose='1'
mqtt_server="mqtt.my.tld"
mqtt="-F mqtt://$mqtt_server:1883,retain=0"
logging="$mqtt -F log -F kv -F json:rtl_433.json"
sample_rate="-s 2048k"
config="/etc/rtl_433.conf"
exclude_from_all='10[67]'

arg=''

cd $homedir

while [ $verbose -gt 0 ]
do
    arg+='-v '
    verbose=$(( verbose - 1 ))
done

case $protocols in
    DEFAULT)
        ;;
    ALL)
        arg+=$( grep '^[# ]  *protocol ' $config | cut -c2- | awk '{ print "-R " $2 }' | egrep -v "$exclude_from_all" | tr '\n' ' ' )
        ;;
    *)
        protocols=$( echo $protocols )
        for p in $protocols
        do
            arg+="-R $p "
        done
        ;;
esac
rtl_433 $sample_rate $logging $arg
