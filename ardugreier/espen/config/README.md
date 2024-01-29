<!---
vim:ts=4:sw=4:sts=4:et:ai
--->
# Extracting the Arduino config

In Preferences, you'll find a field with "Additional boards manager URLs",
which should look something like this:

    https://arduino.esp8266.com/stable/package_esp8266com_index.json,https://github.com/ambiot/amb1_arduino/raw/master/Arduino_package/package_realtek.com_ameba1_index.json,https://github.com/earlephilhower/arduino-pico/releases/download/global/package_rp2040_index.json,https://raw.githubusercontent.com/Lauszus/Sanguino/master/package_lauszus_sanguino_index.json,https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json

It may list several packages, separated by comma and this script doesn't like
commas, so we'll change that to newlines. If you use vim (or vi or something),
this is easy

    :%s/,/\r/g

This should give you a list something like this, which you save as '''urls.txt'''

    https://arduino.esp8266.com/stable/package_esp8266com_index.json
	https://github.com/ambiot/amb1_arduino/raw/master/Arduino_package/package_realtek.com_ameba1_index.json
	https://github.com/earlephilhower/arduino-pico/releases/download/global/package_rp2040_index.json
	https://raw.githubusercontent.com/Lauszus/Sanguino/master/package_lauszus_sanguino_index.json
	https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json

Then you just run

    ./get-packages.sh

This'll download the packages in the list and put them under the packages/
directory. If this directory doesn't exist, it's created for you.

Chip id is [found here](chip_id.txt) and flash id can be [found here](flash_id.txt).

[//]: # (# https://github.com/rkarlsba/ymse/blob/master/ardugreier/espen/konfig/packages/package_esp32_custom_index.json)
[//]: # (https://github.com/rkarlsba/ymse/blob/master/ardugreier/espen/konfig/packages/package_esp32_custom_index.json)
