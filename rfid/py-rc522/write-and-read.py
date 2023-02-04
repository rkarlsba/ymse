#!/usr/bin/env python3

import RPi.GPIO as GPIO
from mfrc522 import SimpleMFRC522

warning_s = "This may overwrite your RFID thingie. Do you want to continue (y/N)? "
answer_s = input('Do You Want To Continue? ')
if answer_s != 'y' and answer_s != 'Y':
    exit("Coward…")

exit("going on")

writer = SimpleMFRC522()

try:
    text = input('0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789')
    print("Now place your tag to write")
    writer.write(text)
    print("Written")
finally:
    GPIO.cleanup()

