from machine import Pin, Timer
led = Pin(25, Pin.OUT)
timer = Timer()
timer_on = Timer()
timer_off = Timer()

blinkfreq = 2
flashlength = 100 # ms

def blink(timer):
    led.toggle()
    
def ledon(timer):
    led.high()

def ledoff(timer):
    led.low()

# timer.init(freq=blinkfreq, mode=Timer.PERIODIC, callback=blink)
timer_on.init(freq=blinkfreq/3, mode=Timer.PERIODIC, callback=ledon)
timer_off.init(freq=blinkfreq/7, mode=Timer.PERIODIC, callback=ledoff)
