#!/bin/bash
# vim:ts=4:sw=4:sts=4:et:ai:si:fdm=marker
#
# This script checks if a postgres server is healthy running on localhost. It will return:
# "HTTP/1.x 200 OK\r" (if postgres is running smoothly)
# - OR -
# "HTTP/1.x 500 Internal Server Error\r" (else)
# The purpose of this script is make haproxy capable of monitoring postgres properly
# It is recommended that a low-privileged postgres  user is created to be used by this script.
# For eg. create  user healthchkusr login password 'hc321';
#
# Changed May 2023 by Roy Sigurd Karlsbakk <roysk@oslomet.no> to also send Content-Length to
# avoid errors indicating connection reset by peer.
#
# Changed July 2025 by Roy Sigurd Karlsbakk <roysk@oslomet.no>, fixing the script where it failed
# when $VALUE wasn't set. Also, added checks for exitcode if the query doesn't return t or f

PGBIN="psql"
PGSQL_HOST="localhost"
PGSQL_PORT="5432"
PGSQL_DATABASE="postgres"
PGSQL_USERNAME="clustercheckuser"
export PGPASSWORD="wwwd0t0sl0metd0tn0"
HEADER_FILE=$( mktemp )
CONTENT_FILE=$( mktemp )

# We perform a simple query that should return a few results
VALUE=$( $PGBIN -t -h $PGSQL_HOST -U $PGSQL_USERNAME -p $PGSQL_PORT -c"select pg_is_in_recovery()" $PGSQL_DATABASE 2> /dev/null )

# psql returns 0 to the shell if it finished normally, 1 if a fatal error of
# its own occurs (e.g. out of memory, file not found), 2 if the connection to
# the server went bad and the session was not interactive, and 3 if an error
# occurred in a script and the variable ON_ERROR_STOP was set.
EXITCODE=$?

# Check the exitcode and output. If it is not empty then everything is fine and
# we return something. Else, we just do not return anything.
case "$VALUE" in
    t)
        printf "HTTP/1.1 206 OK\r\n" >> $HEADER_FILE
        printf "Standby" >> $CONTENT_FILE
        ;;
    f)
        printf "HTTP/1.1 200 OK\r\n" >> $HEADER_FILE
        printf "Primary" >> $CONTENT_FILE
        ;;
    *)
        printf "HTTP/1.1 503 Service Unavailable\r\n" >> $HEADER_FILE
        case "$EXITCODE" in
            0)
                printf "Unknown value returned from db, but good exitcode from psql, weird!\r\n" >> $CONTENT_FILE
                ;;
            1)
                printf "Fatal error: OOM, file not found, some god's wrath\r\n" >> $CONTENT_FILE
                ;;
            2)
                printf "DB Down\r\n" >> $CONTENT_FILE
                ;;
            3)
                printf "Script error\r\n" >> $CONTENT_FILE
                ;;
            *)
                printf "This *really* shouldn't be return by psql ($EXITCODE)\r\n" >> $CONTENT_FILE
                ;;
        esac
esac

printf "\r\n" >> $CONTENT_FILE
content_length=$( wc -c $CONTENT_FILE | awk '{ print $1 }' )

printf "Content-Type: text/plain\r\n" >> $HEADER_FILE
printf "Content-Length: $content_length\r\n" >> $HEADER_FILE
printf "\r\n" >> $HEADER_FILE

cat $HEADER_FILE $CONTENT_FILE

rm -f $HEADER_FILE $CONTENT_FILE
