#!/bin/bash
POSITIONAL_ARGS=()

# i zabbix:
#
# single interface
# rolf-hostnavnsjekk.sh["conn={HOST.CONN}","ip={HOST.IP}","dns={HOST.DNS}","host={HOST.HOST}","hostid={HOST.ID}"]
#
# multiple interfaces check:
# rolf-hostnavnsjekk.sh["conn={HOST.CONN}","ip={HOST.IP}","dns={HOST.DNS}","host={HOST.HOST}","conn1={HOST.CONN1}","ip1={HOST.IP1}","dns1={HOST.DNS1}","host1={HOST.HOST1}","conn2={HOST.CONN2}","ip2={HOST.IP2}","dns2={HOST.DNS2}","host2={HOST.HOST2}","conn3={HOST.CONN3}","ip3={HOST.IP3}","dns3={HOST.DNS3}","host3={HOST.HOST3}","conn4={HOST.CONN4}","ip4={HOST.IP4}","dns4={HOST.DNS4}","host4={HOST.HOST4}","conn5={HOST.CONN5}","ip5={HOST.IP5}","dns5={HOST.DNS5}","host5={HOST.HOST5}","conn6={HOST.CONN6}","ip6={HOST.IP6}","dns6={HOST.DNS6}","host6={HOST.HOST6}","conn7={HOST.CONN7}","ip7={HOST.IP7}","dns7={HOST.DNS7}","host7={HOST.HOST7}","conn8={HOST.CONN8}","ip8={HOST.IP8}","dns8={HOST.DNS8}","host8={HOST.HOST8}","conn9={HOST.CONN9}","ip9={HOST.IP9}","dns9={HOST.DNS9}","host9={HOST.HOST9}","hostid={HOST.ID}"]


#    {HOST.ID}
#        {HOSTGROUP.ID}
#	    {MAP.ID}

#    {HOST.IP} (or its deprecated version {IPADDRESS})
#    {HOST.PORT}
#    {HOST.HOST} (or its deprecated version {HOSTNAME})
#    {HOST.NAME}
#    {HOST.CONN[1-9]}
#    {HOST.DNS}
#    {HOST.IP} (or its deprecated version {IPADDRESS})

	    

# alternative dns server:
# --dnsserver 2001:700:700:1::21

# defaults
dnsserver="2001:700:700:1::21 +timeout=0 +retry=0"

for i in "" {1..9};
do
	eval "port$i=10050"
done
verbose=0
domains="oslomet.no|hioa.no|hio.no"


test_all=0
test_ok=0
test_error=0
test_fail=0
test_warning=0
test_critical=0
info_critical=""
info_warning=""
info_error=""
info_ok=""
info_fail=""

# annet rabbel
# fjern det før = eksempel $b = "-abc=123", da blir ${b#*=} til 123
# ${i#*=}

while [[ $# -gt 0 ]]; do
  case $1 in
	conn*|port*|host*|ip*|dns*)
		#eval ${1##-}
		# stygg, men enkel. Bare eval når den matcher
		echo "$1" | egrep -q "[a-z][0-9]*=[a-zA-Z0-9\.]*" && eval $1
		shift
		;;
    --verbose|-v)
	    verbose=1
	    shift
	    ;;
    --help|-h)
	    echo "Usage: $0 with arguments:"
	    echo "conn<1-9>=		{HOST.CONN<1-9>} default connect for host. Is one of HOST.DNS or HOST.IP"
	    echo "port<1-9>=		{HOST.PORT<1-9>} what port is zabbix_agent on"
	    echo "host<1-9>=		{HOST.HOST<1-9>}"
	    echo "dns<1-9>=		{HOST.DNS<1-9>}"
	    echo "ip<1-9>=		{HOST.IP<1-9>}"
	    echo ""
	    echo "          <1-9> can also be nothing ex $0 conn={HOST.CONN} conn1={HOST.CONN1} ip1={HOST.IP1}"
	    echo ""
	    echo "-ns|--dnsserver 	use dns server for lookups"
	    echo "-h|--help 		this info"
	    echo "-v|--verbose 		show variables"
	    shift
	    ;;
    -*|--*)
      echo "Unknown option $1 use --help for info"
      exit 1
      ;;
    *)
      POSITIONAL_ARGS+=("$1") # save positional arg
      shift # past argument
      ;;
  esac
done
set -- "${POSITIONAL_ARGS[@]}" # restore positional parameters


# FUNCTIONS
function v() { echo $*; $* ; }
function eq() { [ 1 = "$(printf %s\\0 "$@" | sort -uz | tr -cd \\0 | wc -c)" ] ; }

# sjekker om det er ip argument: 0 betyr hostnavn, 1 for ipv4 eller ipv6
function isip() { ipcalc -s -c $1 && echo 1 || echo 0 ; }
function isip4() { ipcalc -s -4 -c $1 && echo 1 || echo 0 ; }
function isip6() { ipcalc -s -6 -c $1 && echo 1 || echo 0 ; }

function add_critical() { info_critical="$info_critical $*"; let test_critical+=1; }
function add_fail() { info_fail="$info_fail $*"; let test_fail+=1; }
function add_warning() { info_warning="$info_warning $*"; let test_warning+=1; }
function add_info() { info_info="$info_info $*"; }

function returninfo()
{
	info=""
	test -n "$info_critical" && info="CRITICAL:$info_critical "
	test -n "$info_fail" && info="${info}FAIL:$info_fail "
	test -n "$info_error" && info="${info}ERROR:$info_error "
	test -n "$info_warning" && info="${info}WARNING:$info_warning"
	test -n "$info_info" && info="${info}INFO:$info_info"
	let test_all="${test_ok}+${test_critical}+${test_fail}+${test_error}+${test_warning}"
	info="$info / tests ok${test_ok} c${test_critical} f${test_fail} e${test_error} w${test_warning} tot${test_all}"
	test $test_critical -gt 0 && echo $info && exit 1
	test $test_fail -gt 0 && echo $info && exit 1
	test $test_error -gt 0 && echo $info && exit 1
	test $test_warning -gt 0 && echo $info && exit 1
	echo "OK $info" 
	exit 0
}




# VERBOSE
if [ $verbose -eq 1 ];
then
	echo ""
	echo "###### VERBOSE ######"
	echo "nr conn:port (dns/ip) host"
	echo -n " "
	for i in "" {1..9};
	do

		echo $i:  $(eval echo "\$conn$i:\$port$i \(\$dns$i/\$ip$i\) \$host$i")
	done
	echo ""
fi

# quit hvis ingen conn er satt
echo $conn$conn{1..9} | grep ^$ && echo "No server to connect to. Try $0 --help" && exit 1

# Fingerprint alle zabbix interfacer på maskinen:
for interface in "" {1..9};
do
	i_conn_dns=""
	i_conn_ip=""
	i_conn=$(eval echo "\$conn$interface")
	i_port=$(eval echo "\$port$interface")
	i_host=$(eval echo "\$host$interface")
	i_ip=$(eval echo "\$ip$interface")
	i_dns=$(eval echo "\$dns$interface")
	server=$i_conn

	t=0  # internal var for timeout. Hvis 1, skipp resten av zabbix_get commandoer for samme host. 0 = ingen timeout, 1 = timeout(skips test)


	if [ \! -z $i_conn ];
	then
		test -z $i_host && add_fail "Fikk ikke host=$i_host fra zabbix sjekken. $0 --help for info." && returninfo

		test "$i_conn" == "$i_ip" && i_conn_dns=0
		test "$i_conn" == "$i_ip" && i_conn_ip=1
		test "$i_conn" == "$i_dns" && i_conn_dns=1
		test "$i_conn" == "$i_dns" && i_conn_ip=0

		args=" -p $i_port"
		domain=$(echo $i_host | sed s/^[^\.]*\.//gi )


		i_system_hostname=$(timeout 2s zabbix_get -s $i_conn $args -k system.hostname 2>&1)
		test $? -ne 0 && add_fail "no agent connection: zabbix_get -s $i_conn $args -k system.hostname exit $RET: ${i_system_hostname}." && returninfo

		let test_ok+=1
                echo $i_system_hostname | egrep -q "$domain|$domains|^$" || add_info "(added .$domain to $i_system_hostname)"
                echo $i_system_hostname | egrep -q "$domain|$domains|^$" || i_system_hostname=$i_system_hostname.$domain

		eq "${i_host,,}" "${i_system_hostname,,}" && let test_ok+=1 || add_critical "(rename $i_host i zabbix til ${i_system_hostname}?)"

# FINGERPRINTING

		# hent en egenkomponert md5 av macaddresse og hostname for hver av connectene. Noe som ligner UUID for maskinen
		lasthost=""
		checksum_hostlist=$(
		  for h in $i_conn $i_host $i_ip $i_dns
		  do
			  t=0  
		          echo -n "$h " ; ( for a in agent.hostname system.hw.macaddr system.hostname ; do test $t -eq 0 && lasthost=$h ; test $t -eq 0 && timeout 2s zabbix_get -s $h $args -k $a 2>&1 ; test $? -eq 124 && t=1 ; done) | md5sum | awk '{print $1}'
			  test $t -eq 1 &&  add_warning "zabbix_get TIMEOUT on $lasthost" ;
		  done
		)
		uniqness_lc=$(printf "$checksum_hostlist" | awk '{print $2}' | sort | uniq -c | wc -l | awk '{print $1}')

		if [ \! "x$uniqness_lc" ==  "x1" ];
		then
			add_critical "$i_host interface($interface) ip/dns/navn treffer ikke samme server $(printf "${checksum_hostlist,,}" | sort | uniq -c | awk '{print $2}' | xargs)."
			returninfo
			exit 1
		fi
		let test_ok+=1

# TEST CONN resolves to SERVER NAME
		if [ $(isip4 $i_conn) -eq 1 ];
		then
			i_conn_ipv4=$i_conn
			i_conn_ptr4=$(dig -x $i_conn @$dnsserver +short | sed s/\.$//gi | grep -v ".arpa$" | xargs )
			info_info="$info_info i_conn_ptr4=$i_conn_ptr4"
			eq "${i_host,,}" "${i_conn_ptr4,,}" && let test_ok+=1 || add_warning "Host $i_host PTR for $i_conn er ${i_conn_ptr4}." 
		fi
                if [ $(isip6 $i_conn) -eq 1 ];
                then
			i_conn_ipv6=$i_conn
			i_conn_ptr6=$(dig -x $i_conn @$dnsserver +short | sed s/\.$//gi | grep -v ".arpa$" | xargs )
			info_info="$info_info i_conn_ptr6=$i_conn_ptr6"
			eq "${i_host,,}" "${i_conn_ptr6,,}" && let test_ok+=1 || add_warning "Host $i_host PTR for $i_conn er ${i_conn_ptr4}." 
                fi

		# hvis vi dealer med dns. Sjekk ip, og så reverse ptr record:
		if [ \! -z $i_dns ]
		then
			eq "${i_host,,}" "${i_dns,,}" && let test_ok+=1 || add_warning "Host $i_host forskjellig fra interface DNS ${i_dns}." 
			i_dns_ipv4_dig=$(dig A $i_dns @$dnsserver +short | xargs)
			i_dns_ipv6_dig=$(dig AAAA $i_dns @$dnsserver +short | xargs)
			i_dns_ipv4=$(echo $i_dns_ipv4_dig|awk '{print $NF}' | xargs)
			i_dns_ipv6=$(echo $i_dns_ipv6_dig|awk '{print $NF}' | xargs)
			if [ $(isip $i_dns_ipv4 ) -eq 1 ];
			then
				i_dns_ptr4all=$(dig -x $i_dns_ipv4 @$dnsserver +short | sed s/\.$//gi | xargs )
				i_dns_ptr4=$(dig -x $i_dns_ipv4 @$dnsserver +short | sed s/\.$//gi | grep -v ".arpa$" | xargs )
				add_info "i_dns_ipv4=$i_dns_ipv4_dig i_dns_ptr4=$i_dns_ptr4"
				eq "${i_host,,}" "${i_dns_ptr4,,}" && let test_ok+=1 || add_warning "PTR($i_dns_ptr4) av dns er ikke $i_dns"
			fi

			if [ $(isip $i_dns_ipv4 ) -eq 1 ];
			then
				i_dns_ptr6all=$(dig -x $i_dns_ipv6 @$dnsserver +short | sed s/\.$//gi | xargs )
				i_dns_ptr6=$(dig -x $i_dns_ipv6 @$dnsserver +short | sed s/\.$//gi | grep -v ".arpa$" | xargs )
				add_info "i_dns_ipv6=$i_dns_ipv6_dig i_dns_ptr6=$i_dns_ptr6"
			fi

		fi
	fi
done

returninfo

