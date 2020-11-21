ESPTOOL=/usr/local/Cellar/esptool/3.0/bin/esptool.py
PORT=/dev/tty.SLAB_USBtoUART 
FIRMWARE=esp32-idf3-20200902-v1.13.bin
BAUD=460800

all:
	@echo Read me first, please

erase:
	${ESPTOOL} --chip esp32 --port ${PORT} erase_flash

install:
	${ESPTOOL} --chip esp32 --port ${PORT} --baud ${BAUD} write_flash -z 0x1000 ${FIRMWARE}

