#!/usr/bin/env python3

import RPi.GPIO as GPIO
from mfrc522 import SimpleMFRC522

GPIO.setwarnings(False)

warning_s = "This may overwrite your RFID thingie. Do you want to continue (y/N)? "
answer_s = input(warning_s)
if answer_s != 'y' and answer_s != 'Y':
    exit("Coward…")

writer = SimpleMFRC522()

try:
    text = 'abcdefghijklmnopqrstuvwxyz67890123456789012345678901234567890123456789012345678901234567890123456789' # 100
    text = '0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789' # 100
    text += text # 200
    text += text # 400
    text += text # 800
    text += text # 1600
    text += text # 3200
    text += text # 6400
    text += text # 12800
    # text = '[mailto:roy@karlsbakk.net]'
    print(f"Now place your tag to write ('{text}' [{len(text)}])")
    writer.write(text)
    print("Written")
finally:
    GPIO.cleanup()

print("Attempting to read back the data…")

reader = SimpleMFRC522()

try:
    id, text = reader.read()
    print(f"ID:\t{id}\t{hex(id)}")
    print(f"Text:\t'{text}' [{len(text)}]", )
finally:
    GPIO.cleanup()

