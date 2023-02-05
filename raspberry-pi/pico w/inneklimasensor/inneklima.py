# co2-måler basert på MH-Z19-noe og en AHT20 for temp+fukt
import utime
from machine import Pin, I2C
import network
import ubinascii
import gc

# secrets.py should contain something like this:
# wifi_auth = {
#     'ssid': 'mylilnetvork',
#     'pw': 'supasekret'
# }

# These are local
from secrets import wifi_auth
import ahtx0
from umqtt_simple import MQTTClient

# Server names etc
mqtt_server = 'mqtt.karlsbakk.net'
client_id = ubinascii.hexlify(machine.unique_id())
topic_sub = b'notification'
topic_pub = b'hello'

# Connect to the network
wlan = network.WLAN(network.STA_IF)
wlan.active(True)
# The MH-Z19 CO2 sensor will eat far too much power for this to be
# run off battery anyway, so just disable powersave mode to avoid
# any potential issues there.
wlan.config(pm = 0xa11140)
wlan.connect(wifi_auth["ssid"], wifi_auth["pw"])

max_wait = 10
while max_wait > 0:
    if wlan.status() < 0 or wlan.status() >= 3:
        break
    max_wait -= 1
    print('waiting for connection...')
    utime.sleep(1)

# Handle connection error
if wlan.status() != 3:
    raise RuntimeError('wifi connection failed')
else:
    print('connected')
    status = wlan.ifconfig()
    print('ip = ' + status[0])

# We're using I2C #1, leaving #0 for now
i2c = I2C(1, scl=Pin(15), sda=Pin(14))

# Create the sensor object using I2C
sensor = ahtx0.AHT10(i2c)

# Measure and report
while True:
    print("\nTemperature: %0.2f C" % sensor.temperature)
    print("Humidity: %0.2f %%" % sensor.relative_humidity)
    utime.sleep(5)