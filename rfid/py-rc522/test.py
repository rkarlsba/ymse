#!/usr/bin/env python3
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker

import time
from pyrc522 import RFID
import RPi.GPIO as GPIO

GPIO.setwarnings(False)

rdr = RFID()
util = rdr.util()

# util.debug = True
try:
    while True:
        rdr.wait_for_tag()
        (error, data) = rdr.request()
        if not error:
                print("\nDetected")
                (error, uid) = rdr.anticoll()
                if not error:
                        print("Card read UID: " + str(uid))
                        # Set tag as used in util. This will call RFID.select_tag(uid)
                        util.set_tag(uid)
                        util.auth(rdr.auth_a, [0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF])
                        util.deauth()
                        time.sleep(1)

except KeyboardInterrupt:
    print('interrupted!')
    GPIO.cleanup()

