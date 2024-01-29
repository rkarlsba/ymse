#!/bin/bash
#
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    echo "We're on linux!"
elif [[ "$OSTYPE" == "darwin"* ]]; then
    echo "We're on a mac!"
elif [[ "$OSTYPE" == "cygwin" ]]; then
    echo "We're on cygwin!"
elif [[ "$OSTYPE" == "msys" ]]; then
    echo "We're on MinGW?"
elif [[ "$OSTYPE" == "win32" ]]; then
    echo "We're on Win32???"
elif [[ "$OSTYPE" == "freebsd"* ]]; then
    echo "We're on Freebsd <3"
else
    echo "We're on some freaky OS I don't know ($OSTYPE)"
fi
