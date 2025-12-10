#!/usr/bin/env bash
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker:isfname-==

PATH="$PATH:/opt/oslomet/bin"
MAILLOGS='/var/log/mail.log /var/log/maillog'
MAILLOG=''
PFOFFSETFILE='/tmp/zabbix-postfix-offset.dat'
PFSTATSFILE='/tmp/postfix_statsfile.dat'
TEMPFILE=$(mktemp)
PFLOGSUMM='/usr/sbin/pflogsumm'
PYGTAILS='pygtail pygtail.py'
PYGTAIL=''
TEST=0

# list of values we are interested in
PFVALS=( 'received' 'delivered' 'forwarded' 'deferred' 'bounced' 'rejected' 'held' 'discarded' 'reject_warnings' 'bytes_received' 'bytes_delivered' )

trap cleanup EXIT SIGINT SIGTERM SIGHUP SIGQUIT

# write result of running this script
write_result () {
    echo "$2"
    exit $1
}

# Cleanup before exiting
cleanup() {
    rm -f $TEMPFILE
}

# Check for --test
[ $1 == "--test" ] && TEST=1

# check for binaries we need to run the script
if [ ! -x "${PFLOGSUMM}" ]
then
    write_result 1 "ERROR: ${PFLOGSUMM} not found"
fi

for pygtail in $PYGTAILS
do
    pte=$( which ${pygtail} )
    if [ -x "$pte" ]
    then
        PYGTAIL=${pte}
        break
    fi
done

if [ "$PYGTAIL" == "" ]
then
    result_text+="Error: Can't find pygtail (or pygtail.py)\n"
    for pygtail in $PYGTAILS
    do
        result_text+="- $pygtail\n"
    done
    result_code="2"
    write_result "${result_code}" "${result_text}"
fi

for maillog in $MAILLOGS
do
    if [ -r ${maillog} ]
    then
        MAILLOG=${maillog}
        break
    fi
done

if [ "$MAILLOG" == "" ]
then
    result_text="Error: Can't find maillog - looked for these logs:\n"
    for maillog in $MAILLOGS
    do
        result_text+="- $maillog\n" >&2
    done
    result_code="3"
    write_result "${result_code}" "${result_text}"
fi

# check whether file exists and the write permissions are granted
if [ ! -w "${PFSTATSFILE}" ]
then
    touch "${PFSTATSFILE}" && chown zabbix:zabbix "${PFSTATSFILE}" > /dev/null 2>&1

    if [ ! $? -eq 0 ]
    then
        result_text="ERROR: wrong exit code returned while creating file ${PFSTATSFILE} and setting its owner to zaabbix:zabbix"
        result_code="4"
        write_result "${result_code}" "${result_text}"
    fi
fi

# read specific value from data file and print it
readvalue () {
    local key
    key=$(echo ${PFVALS[@]} | grep -wo $1)
    if [ -n "${key}" ]
    then
        value=$(grep -e "^${key};" "${PFSTATSFILE}" | cut -d ";" -f2)
        echo "${value}"

    else
        rm "${TEMPFILE}"
        result_text="ERROR: could not get value \"$1\" from ${PFSTATSFILE}"
        result_code="5"
        write_result "${result_code}" "${result_text}"
    fi
}

# update value  in data file
updatevalue() {
    local key
    local pfkey

    key=$1
    pfkey=$(echo "$1" | tr '_' ' ')

    # convert value to bytes
    value=$(grep -m 1 "$pfkey" $TEMPFILE | awk '{print $1}' | awk '/k|m/{p = /k/?1:2}{printf "%d\n", int($1) * 1024 ^ p}')

    # update values in data file
    old_value=$(grep -e "^${key};" "${PFSTATSFILE}" | cut -d ";" -f2)
    if [ -n "${old_value}" ]
    then
        sed -i -e "s/^${key};${old_value}/${key};$((${old_value}+${value}))/" "${PFSTATSFILE}"
    else
        echo "${key};${value}" >> "${PFSTATSFILE}"
    fi
}

# is there a requests for specific value or do we update all values ?
if [ -n "$1" ]
then
    readvalue "$1"
else
    # read the new part of mail log and read it with pflogsumm to get the summary
    "${PYGTAIL}" -o"${PFOFFSETFILE}" "${MAILLOG}" | "${PFLOGSUMM}" -h 0 -u 0 --no_bounce_detail --no_deferral_detail --no_reject_detail --no_smtpd_warnings --no_no_msg_size > "${TEMPFILE}" 2>/dev/null

    if [ ! $? -eq 0 ]
    then
        result_text="ERROR: wrong exit code returned while running  \"${PYGTAIL}\" -o\"${PFOFFSETFILE}\" \"${MAILLOG}\" | \"${PFLOGSUMM}\" -h 0 -u 0 --no_bounce_detail --no_deferral_detail --no_reject_detail --no_smtpd_warnings --no_no_msg_size > \"${TEMPFILE}\" 2>/dev/null"
        result_code="1"
        write_result "${result_code}" "${result_text}"
    fi

    # update all values from pflogsumm summary
    for i in "${PFVALS[@]}"
    do
        updatevalue "$i"
    done

    result_text="OK: statistics updated"
    result_code="0"
    write_result "${result_code}" "${result_text}"
fi

rm "${TEMPFILE}"

