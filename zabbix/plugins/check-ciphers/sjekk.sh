SERVERS="begrunnelse.hioa.no bibin.hioa.no bibin.oslomet.no camunda-stage.oslomet.no cloud.cs.hioa.no epay.hioa.no epay.oslomet.no provisjon.oslomet.no sites.oslomet.no styringsportal.hioa.no styringsportal.oslomet.no tps.oslomet.no"

for srv in $SERVERS
do
	printf "%25s\t" $srv
	./zabbix_check_ciphers.pl -N /opt/nmap/bin/nmap $srv
	#curl -i $srv 2>/dev/null | grep ^Server || echo
done
