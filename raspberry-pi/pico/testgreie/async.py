from machine import Pin, WDT
import uasyncio
import uos, time

'''
# Setup the watchdog timer
wdt = WDT(timeout=2000)  # enable it with a timeout of 2s
wdt.feed()
'''

# Globals
count = 0

async def blink(led, period_ms):
    led_on = 5;
    while True:
        led.on()
        await uasyncio.sleep_ms(led_on)
        led.off()
        await uasyncio.sleep_ms(period_ms-led_on)

async def main(led):
    uasyncio.create_task(blink(led, 1000))
    await uasyncio.sleep_ms(10000)

# Running on a pyboard
led = Pin(25, Pin.OUT)
uasyncio.run(main(led))

"""
f = open("afile", "w")
f.write("This is a file\n")
f.close()
try:
    uos.remove("bfile")
except:
    pass
    
def blink(timer):
  global count;
  count += 1
  led.toggle()

timer.init(freq=4, mode=Timer.PERIODIC, callback=blink)

while True:
    uos.rename("afile", "bfile")
    time.sleep(0.1111)
    uos.rename("bfile", "afile")
    time.sleep(0.1111)
    print("count =", count)
"""