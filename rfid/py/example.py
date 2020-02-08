#!/usr/bin/python3

from time import sleep
import sys
from mfrc522 import SimpleMFRC522
reader = SimpleMFRC522()

# /usr/local/lib/python3.7/dist-packages/mfrc522/MFRC522.py
# /usr/local/lib/python3.7/dist-packages/mfrc522/SimpleMFRC522.py

try:
    while True:
        print("Hold a tag near the reader")
#       uid, text = reader.read()
#       print("ID: %s\nText: %s" % (hex(uid),text))
#       uid = reader.read_id()
#       print("ID: %x\n" % uid)
        data = reader.read()
        print(data)
        sleep(5)
#except KeyboardInterrupt:
finally:
    GPIO.cleanup()
    raise
