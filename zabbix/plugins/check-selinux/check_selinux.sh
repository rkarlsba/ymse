#!/bin/bash
# vim:ts=4:sw=4:sts=4:et:ai

selinuxstatus=$( getenforce )
case $selinuxstatus in
    Enforcing)
        echo "OK: SELinux is enforcing security"
        ;;
    Permissive)
        echo "WARNING: SELinux is set to permissive mode"
        ;;
    Disabled)
        echo "ERROR: SELinux is disabled"
        ;;
    *)
        echo "UNKNWON: getenforce returned $selinuxstatus of which I don't know a thing"
        ;;
esac

