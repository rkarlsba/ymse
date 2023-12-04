#!/bin/bash
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker
#
# This file uses vim folds. If you're confused, press zR in command mode to
# open all folds. To learn more, try :help fold
#
# RHEL7 example from admin-stage {{{ 
#
# NAME="Red Hat Enterprise Linux Server"
# VERSION="7.9 (Maipo)"
# ID="rhel"
# ID_LIKE="fedora"
# VARIANT="Server"
# VARIANT_ID="server"
# VERSION_ID="7.9"
# PRETTY_NAME="Red Hat Enterprise Linux"
# ANSI_COLOR="0;31"
# CPE_NAME="cpe:/o:redhat:enterprise_linux:7.9:GA:server"
# HOME_URL="https://www.redhat.com/"
# BUG_REPORT_URL="https://bugzilla.redhat.com/"
# 
# REDHAT_BUGZILLA_PRODUCT="Red Hat Enterprise Linux 7"
# REDHAT_BUGZILLA_PRODUCT_VERSION=7.9
# REDHAT_SUPPORT_PRODUCT="Red Hat Enterprise Linux"
# REDHAT_SUPPORT_PRODUCT_VERSION="7.9"
#
# }}}
# CentOS7 example from admin-stage {{{ 
#
# NAME="CentOS Linux"
# VERSION="7 (Core)"
# ID="centos"
# ID_LIKE="rhel fedora"
# VERSION_ID="7"
# PRETTY_NAME="CentOS Linux 7 (Core)"
# ANSI_COLOR="0;31"
# CPE_NAME="cpe:/o:centos:centos:7"
# HOME_URL="https://www.centos.org/"
# BUG_REPORT_URL="https://bugs.centos.org/"
# 
# CENTOS_MANTISBT_PROJECT="CentOS-7"
# CENTOS_MANTISBT_PROJECT_VERSION="7"
# REDHAT_SUPPORT_PRODUCT="centos"
# REDHAT_SUPPORT_PRODUCT_VERSION="7"
#
# }}}
# Debian 12 example from smilla {{{ 
#
# PRETTY_NAME="Debian GNU/Linux 12 (bookworm)"
# NAME="Debian GNU/Linux"
# VERSION_ID="12"
# VERSION="12 (bookworm)"
# VERSION_CODENAME=bookworm
# ID=debian
# HOME_URL="https://www.debian.org/"
# SUPPORT_URL="https://www.debian.org/support"
# BUG_REPORT_URL="https://bugs.debian.org/"
#
# }}}

version_file='/etc/os-release'

if [ ! -r $version_file ]
then
    echo "Can't read version file '$version_file': $!" 2>&1
    exit 1
fi

distro_type=$( awk -F= '/^\<ID\>/ { print $2 }' $version_file | sed 's/"//g' )
distro_version=$( awk -F= '/^\<VERSION_ID\>/ { print $2 }' $version_file | sed 's/"//g' | sed 's/\..*//g')

case $distro_type in
    'rhel')
        distro_type='RHEL'
        ;;
    'centos')
        distro_type='CentOS'
        ;;
    'debian')
        distro_type='Debian'
        ;;
    *)
        distro_type="*$distro_type*"
        ;;
esac

distronameandversion="$distro_type$distro_version"
distronameandversioncoloured="\e[0;31m$distronameandversion\e[m"

if [[ "$TERM" =~ 256color ]]
then
    distroprompt="$distronameandversioncoloured $PS1"
else
    distroprompt="$distronameandversion $PS1"
fi

echo $distroprompt
exit 0
