from machine import Pin, Timer
from time import sleep
import os

count = 0
count_max = 20
state = 0
frequency = 20
led = Pin(25, Pin.OUT)

timer = Timer()
logdir = 'var/log'

def mkvarlog():
    logdirparts = logdir.split('/')
    print(logdirparts)
    for count in range(len(logdirparts)):
        print(logdirparts[count])
        if not logdirparts[count] in os.listdir():
            os.mkdir(logdirparts[count])
        os.chdir(logdirparts[count])
    os.chdir('/')
    os.listdir()
    

def blink(timer):
    global led
    global state
    global count
    global count_max
    
    if count < count_max:
        led.off()
    else:
        led.on()
        
    if count >= count_max:
        count=0;
        
    count+=1;
    

timer.init(freq=frequency, mode=Timer.PERIODIC, callback=blink)

'''
while True:
    led.on()
    sleep(0.1)
    led.off()
    sleep(0.9)
 '''   
 
