#!/bin/bash
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker

DEBUG=0
if [ $DEBUG -gt 0 ]
then
    exec 2>>/tmp/my.log
    set -x
fi

f=$1
servername=$2
port=$3
hostip=$4
proto=$5

if [ -z "$hostip" ]
then
    host=$servername
else
    host=$hostip
fi

if [ -n "$proto" ]
then
    starttls="-starttls $proto"
fi

case $f in
    -d|-e|-I)
        end_date=$( openssl s_client -servername $servername -host $host -port $port -showcerts $starttls -prexit </dev/null 2>/dev/null |
            sed -n '/BEGIN CERTIFICATE/,/END CERT/p' 2>/dev/null |
            openssl x509 -text 2>/dev/null |
            sed -n 's/ *Not After : *//p' 2>/dev/null )

        if [ -n "$end_date" ]
        then
            end_date_seconds=$( date '+%s' --date "$end_date" )
            now_seconds=$( date '+%s' )

            case $f in
                -d)
                    echo "$(( ($end_date_seconds-$now_seconds)/24/3600 ))"
                    #echo "($end_date_seconds-$now_seconds)/24/3600" | bc
                    ;;
                -e)
                    echo $end_date_seconds
                    ;;
                -I)
                    end_date_iso=$( date --date "$end_date" +%Y-%m-%d\ %H:%M:%S )
                    echo $end_date_iso
                    ;;
            esac
            exit 0
        fi
        ;;
    -i)
        issue_dn=$( openssl s_client -servername $servername -host $host -port $port -showcerts $starttls -prexit </dev/null 2>/dev/null |
            sed -n '/BEGIN CERTIFICATE/,/END CERT/p' 2>/dev/null |
            openssl x509 -text 2>/dev/null |
            sed -n 's/ *Issuer: *//p' 2>/dev/null )

        if [ -n "$issue_dn" ]
        then
            issuer=$( echo $issue_dn | sed -n 's/.*CN=*//p' 2>/dev/null )
            echo $issuer
        fi
        exit 0
        ;;
    -m)
        echo "Not implemented" >&2
        exit 1
        echo -n | openssl s_client -connect google.com:443 -servername ibm.com | openssl x509 -noout -text | grep DNS
        ;;
    *)
        cat <<EOT
Usage: $0 [-i|-d|-e|-I] hostname port sni
    -i  Show Issuer
    -d  Show valid days remaining
    -e  Show expiry date in epoch format (seconds after 1970-01-01 00:00:00)
    -I  Show expiry date in ISO format (YYYY-MM-DD hh:mm:ss)
    -m  Check if certificate is valid for given hostname
EOT
        ;;
esac
