#!/usr/bin/env python3
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker

import RPi.GPIO as GPIO
from mfrc522 import SimpleMFRC522

GPIO.setwarnings(False)

reader = SimpleMFRC522()

try:
    id, text = reader.read()
    print(f"ID:\t{id}\t{hex(id)}")
    print(f"Text:\t'{text}' [{len(text)}]", )
finally:
    GPIO.cleanup()

# print("don't start by writing...")
