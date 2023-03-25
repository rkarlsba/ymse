#!/bin/bash
# vim:ts=4:sw=4:sts=4:et:ai

USB_DEVICE='/dev/tty.usbserial-14320'

case "$?" in
    1)
        if [ -c "$1" = "/dev/" ]
        then
            USB_DEVICE="$1"
        else
            echo "$1" is not a character device
            exit 1
        fi
        ;;
    *)
        echo "Invalid number of arguments" >&2
        exit 2
esac

# Slett gammel flash
esptool.py -p $USB_DEVICE erase_flash --flash_size auto

# Skriv ny
esptool.py -p $USB_DEVICE write_flash --flash_size auto 0x0 WLED_0.14.0-b1_ESP8266.bin
