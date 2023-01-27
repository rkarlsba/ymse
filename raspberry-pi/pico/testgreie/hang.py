from machine import Pin, Timer, WDT
import uos, time
import gc

# Setup the watchdog timer
wdt = WDT(timeout=2000)  # enable it with a timeout of 2s
wdt.feed()

gc.disable()

led = Pin(25, Pin.OUT)
timer = Timer()

count = 0

f = open("afile", "w")
f.write("This is a file\n")
f.close()
try:
    uos.remove("bfile")
except:
    pass
    
def blink(timer):
  global count;
#  count += 1
  led.toggle()

timer.init(freq=4, mode=Timer.PERIODIC, callback=blink)

while True:
    uos.rename("afile", "bfile")
    time.sleep(0.1111)
    uos.rename("bfile", "afile")
    time.sleep(0.1111)
    print("count =", count)
