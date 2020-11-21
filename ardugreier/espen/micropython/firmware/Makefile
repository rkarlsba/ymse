# vim:tw=80
#
# Simple makefile to do the dirty stuff. Just change whatever's needed befor
# using. See https://micropython.org/download/esp32/ for downloadable firmware.
# Place this file under the images/ dir and update filenames wanted below. Run
# 'make erase' and 'make install'.
#
# Have fun :)
# 
ESPTOOL=/usr/local/Cellar/esptool/3.0/bin/esptool.py
PORT=/dev/tty.SLAB_USBtoUART 
BAUD=460800
STABLE_FIRMWARE=images/esp32-idf3-20200902-v1.13.bin
PSYCHO_FIRMWARE=images/esp32-idf3-20201121-unstable-v1.13-180-g5af3c046c.bin

FIRMWARE=${PSYCHO_FIRMWARE}

all:
	@echo Read me first, please

erase:
	${ESPTOOL} --chip esp32 --port ${PORT} erase_flash

install:
	${ESPTOOL} --chip esp32 --port ${PORT} --baud ${BAUD} write_flash -z 0x1000 ${FIRMWARE}

