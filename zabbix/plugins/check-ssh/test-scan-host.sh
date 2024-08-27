#!/bin/bash
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker

host=u804.karlsbakk.net
username=zabbix
port=22

outdir="host-results"
outfile="$outdir/algos_${host}.txt"

if [ -f $outfile ]
then
    printf "Output file '%s' already exists - giving up\n" "${outfile}"
    exit 1
fi

nmap -p ${port} --script ssh2-enum-algos --script-args="ssh.user=${username}" ${host} > $outfile
