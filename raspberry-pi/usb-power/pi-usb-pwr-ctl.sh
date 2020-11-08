#!/bin/sh
# vim:ts=4:sw=4:sts=4:et:ai

echo "This doesn't seem to work at all - no idea why" >&2
exit 1

SOC_USB=/sys/devices/platform/soc/20980000.usb

if [ ! -d $SOC_USB ];
then
    SOC_USB=/sys/devices/platform/soc/3f980000.usb # Raspberry Pi 3
fi

BUSPOWER=$SOC_USB/buspower

is_usb_power_on () {
    cat $BUSPOWER | grep "Bus Power = 0x1" >/dev/null
}

case $1 in
    stop)
        if is_usb_power_on
        then
            echo 0x0 > $BUSPOWER 
        fi
        ;;
    start)
        if ! is_usb_power_on
        then
            echo 0x1 > $BUSPOWER
        fi
        ;;
    status)
        if is_usb_power_on
        then
            echo "USB power is on"
        else
            echo "USB power is off"
        fi
        ;;
    *)
        echo "Usage: $0 start|stop|status" >&2
        exit 2
        ;;
esac

exit 0
