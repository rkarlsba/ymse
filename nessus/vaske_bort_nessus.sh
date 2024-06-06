#!/bin/bash
# Docs {{{
# 
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker:tw=80
#
# Vaske loggfiler til Apache og tilsvarende for å få fjerna driten til Nessus,
# som alltid forurenser loggene med skrap. Vi filtrerer her på IP-adressa til en
# eller flere kjente nessus-maskiner.
#
# Written by Roy Sigurd Karlsbakk <roy@karlsbakk.net> CopyLeft 2024.
# Licensed under som obscure BSD license of choice
#
# }}}
# {{{ ls /var/log/httpd/ 
# 
# access_log          ssl_access_log          synopsis.oslomet.no-access_log           synopsis.oslomet.no-error_log-20240519       synopsis.oslomet.no-ssl_access_log-20240602
# error_log           ssl_error_log           synopsis.oslomet.no-access_log-20240512  synopsis.oslomet.no-error_log-20240526       synopsis.oslomet.no-ssl_error_log
# error_log-20240512  ssl_error_log-20240512  synopsis.oslomet.no-access_log-20240519  synopsis.oslomet.no-error_log-20240602       synopsis.oslomet.no-ssl_error_log-20240512
# error_log-20240519  ssl_error_log-20240519  synopsis.oslomet.no-access_log-20240526  synopsis.oslomet.no-ssl_access_log           synopsis.oslomet.no-ssl_error_log-20240519
# error_log-20240526  ssl_error_log-20240526  synopsis.oslomet.no-access_log-20240602  synopsis.oslomet.no-ssl_access_log-20240512  synopsis.oslomet.no-ssl_error_log-20240526
# error_log-20240602  ssl_error_log-20240602  synopsis.oslomet.no-error_log            synopsis.oslomet.no-ssl_access_log-20240519  synopsis.oslomet.no-ssl_error_log-20240602
# gml/                ssl_request_log         synopsis.oslomet.no-error_log-20240512   synopsis.oslomet.no-ssl_access_log-20240526
# 
# }}}
# function show_help {{{

function show_help {
    [ $# -gt 0 ] && echo $@
    echo <<EOT
Syntax: $0 [ -b | -g | -x ] [ -h ] [ -f ]

Having
    -b      == Compress backups with bzip2
    -g      == Compress backups with gzip
    -x      == Compress backups with xz

    -h / -? == Show this help

    -f      == Forcibly overwrite old backup files with the same name as new ones
EOT
    exit 0
}

# }}}
# function file_is_empty {{{

function file_is_empty {
    filename=$1

    if [ -f $filename -a ! -s $filename ]
    then
        echo "File $filename exists, seems to be an ordinary file and is zero bytes long"
        return 1
    fi
    return 0
}

# }}}
# function show_help {{{

function show_help {
    [ $# -gt 0 ] && echo $@
    echo <<EOT
Syntax: $0 [ -b | -g | -x ] [ -h ] [ -f ]

Having
    -b      == Compress backups with bzip2
    -g      == Compress backups with gzip
    -x      == Compress backups with xz

    -h / -? == Show this help

    -f      == Forcibly overwrite old backup files with the same name as new ones
EOT
    exit 0
}

# }}}
# Global Variables {{{

#dir='/var/log/httpd'
dir='/var/log/httpd/test'
files='*_log*'
nessus='\[client (158\.36\.68\.|2001:700:700:2002::)79'
debug=0
verbose=0
optstr=":dhv?"

# }}}
# getopts {{{

while getopts ${optstr} opt
do
    case ${opt} in
        'd')
            debug=1
            ;;
        'h'|'?')
            show_help
            ;;
        'v')
            verbose=1
            ;;
        *)
            echo "Invalid option: -${OPTARG}."
            exit 2
            ;;
    esac
done
shift $( expr $OPTIND - 1 )

if [ $debug -gt 0 ]
then
    echo "Number of args: $# (verbose is $verbose)"
    exit 0
fi

# }}}
# Resten {{{

cd $dir
for f in $files
do
    perl -ne "print unless /$nessus/;" -i $f
    if (-z
sed -i $files "s/$nessus//"

# }}}
