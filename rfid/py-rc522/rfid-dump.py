#!/usr/bin/env python3
#
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker
#
#              .';:cc;.
#            .,',;lol::c.
#            ;';lddddlclo
#            lcloxxoddodxdool:,.
#            cxdddxdodxdkOkkkkkkkd:.
#          .ldxkkOOOOkkOO000Okkxkkkkx:.
#        .lddxkkOkOOO0OOO0000Okxxxxkkkk:
#       'ooddkkkxxkO0000KK00Okxdoodxkkkko
#      .ooodxkkxxxOO000kkkO0KOxolooxkkxxkl
#      lolodxkkxxkOx,.      .lkdolodkkxxxO.
#      doloodxkkkOk           ....   .,cxO;
#      ddoodddxkkkk:         ,oxxxkOdc'..o'
#      :kdddxxxxd,  ,lolccldxxxkkOOOkkkko,
#       lOkxkkk;  :xkkkkkkkkOOO000OOkkOOk.
#        ;00Ok' 'O000OO0000000000OOOO0Od.
#         .l0l.;OOO000000OOOOOO000000x,
#            .'OKKKK00000000000000kc.
#               .:ox0KKKKKKK0kdc,.
#                      ...
#
# Author: peppe8o
# Date: Jul 3rd, 2022
# Version: 1.0
# https://peppe8o.com

import time
from pyrc522 import RFID
import RPi.GPIO as GPIO

rdr = RFID()
util = rdr.util()
# Set util debug to true - it will print what's going on
# util.debug = True
try:
    while True:
        # Wait for tag
        rdr.wait_for_tag()
        # Request tag
        (error, data) = rdr.request()
        if not error:
            print("\nDetected")
            (error, uid) = rdr.anticoll()
            if not error:
                # Print UID
                print("Card read UID: " + str(uid))
                # Set tag as used in util. This will call RFID.select_tag(uid)
                util.set_tag(uid)
                # Save authorization info (key A) to util. It doesn't call RFID.card_auth(), that's called when needed
                util.auth(rdr.auth_a, [0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF])
                util.dump()
                # Stop crypto
                util.deauth()
                print("Dump completed, waiting 1 sec for next reading")
                time.sleep(1)
                print("Available to start a new reading")

except KeyboardInterrupt:
    print('interrupted!')
    GPIO.cleanup()
