#!/bin/bash
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker

if [ $# -eq 0 ]
then
    echo "Syntax: $0 filename [filename [ filename [ … ]]]" >&2
    exit 1
fi

for srv in $@
do
	printf "%25s\t" $srv
	./zabbix_check_ciphers.pl -N /opt/nmap/bin/nmap $srv
	#curl -i $srv 2>/dev/null | grep ^Server || echo
done
