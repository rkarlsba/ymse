#!/bin/sh
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
        end_date=`openssl s_client -servername $servername -host $host -port $port -showcerts $starttls -prexit </dev/null 2>/dev/null |
            sed -n '/BEGIN CERTIFICATE/,/END CERT/p' |
            openssl x509 -text 2>/dev/null |
            sed -n 's/ *Not After : *//p'`

        if [ -n "$end_date" ]
        then
            end_date_seconds=`date '+%s' --date "$end_date"`
            now_seconds=`date '+%s'`

            case $f in
                -d)
                    echo $(( ($end_date_seconds-$now_seconds)/24/3600 ))
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
        fi
        ;;
    -i)
        issue_dn=`openssl s_client -servername $servername -host $host -port $port -showcerts $starttls -prexit </dev/null 2>/dev/null |
            sed -n '/BEGIN CERTIFICATE/,/END CERT/p' |
            openssl x509 -text 2>/dev/null |
            sed -n 's/ *Issuer: *//p'`

        if [ -n "$issue_dn" ]
        then
            issuer=`echo $issue_dn | sed -n 's/.*CN=*//p'`
            echo $issuer
        fi
        ;;
    *)
        echo "usage: $0 [-i|-d|-e|-I] hostname port sni"
        echo "    -i Show Issuer"
        echo "    -d Show valid days remaining"
        echo "    -e Show expiry date in epoch format (seconds after 1970-01-01 00:00:00)"
        echo "    -I Show expiry date in ISO format (YYYY-MM-DD hh:mm:ss)"
        ;;
esac
