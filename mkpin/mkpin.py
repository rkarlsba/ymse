#!/usr/bin/env python3

import random
import sys

default_digits = 3

def syntax(name):
    print(f"Syntax: {name} [digits]")
    print(f"\nExample:\n\t{name}\tReturn a {default_digits}-digit pin.\n\t{name} 42\tReturn a 42-digit pin.")
    exit(0)

# Get digit count or just default to default_digits
if (len(sys.argv) == 1):
    digits = default_digits
elif (len(sys.argv) == 2):
    digits = int(sys.argv[1])
else:
    syntax(sys.argv[0])

devurandom = '/dev/urandom'
seedlen = 16
pin = ''

try:
    ur = open(devurandom, 'rb')
    randseed = ur.read(seedlen)

except OSError:
    print("Could not open/read file \"{devurandom\"}")
    sys.exit(1)

finally:
    ur.close()

random.seed(randseed)
pin=''
for c in range(digits):
    r =  random.randint(0, 9)
    pin = f'{pin}{r}'

print(f"Got pin code {pin} for you, sire!")
