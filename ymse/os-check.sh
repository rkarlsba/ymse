#!/bin/bash
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker

case "$OSTYPE" in
    "linux-gnu"*)
        echo "We're on Linux! ($OSTYPE)"
        ;;
    "darwin"*)
        echo "We're on a mac! ($OSTYPE)"
        ;;
    "cygwin")
        echo "We're on Cygwin! ($OSTYPE)"
        ;;
    "msys")
        echo "We're on MinGW? ($OSTYPE)"
        ;;
    "win32")
        echo "We're on Win32??? ($OSTYPE)"
        ;;
    "freebsd"*)
        echo "We're on Freebsd <3 ($OSTYPE)"
        ;;
    *)
        echo "We're on some freaky OS I don't know ($OSTYPE)"
        ;;
esac
exit

if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    echo "We're on linux! ($OSTYPE)"
elif [[ "$OSTYPE" == "darwin"* ]]; then
    echo "We're on a mac! ($OSTYPE)"
elif [[ "$OSTYPE" == "cygwin" ]]; then
    echo "We're on cygwin! ($OSTYPE)"
elif [[ "$OSTYPE" == "msys" ]]; then
    echo "We're on MinGW? ($OSTYPE)"
elif [[ "$OSTYPE" == "win32" ]]; then
    echo "We're on Win32??? ($OSTYPE)"
elif [[ "$OSTYPE" == "freebsd"* ]]; then
    echo "We're on Freebsd <3 ($OSTYPE)"
else
    echo "We're on some freaky OS I don't know ($OSTYPE)"
fi
