from machine import Pin, Timer
from collections import namedtuple
import os, time

led  = Pin(25, Pin.OUT)

PoolItem  = namedtuple('PoolItem', ['func', 'args', 'loop'])
func_pool = []
ticks     = 0

def timer_pool(t:Timer) -> None:
    global ticks
    for pi in func_pool[:]:
        if not pi.loop:
            func_pool.remove(pi)
        elif pi.loop in (True, False) or not ticks%pi.loop:
            pi.func() if not pi.args else pi.func(*pi.args)
    ticks += 1
        
def toggle_led():
    led.toggle()
    
def log(d:str):
    if os.getcwd() != '/{}'.format(d):
        if not d in os.listdir():
            os.mkdir(d)
        os.chdir(d)
        
    with open('logs.txt', 'a') as f:
        f.write('logged @: {}\n'.format(time.localtime()))
        
    
func_pool.append(PoolItem(toggle_led, None, True))
func_pool.append(PoolItem(log, ('logs', ), 20))
  
Timer().init(freq=4, mode=Timer.PERIODIC, callback=timer_pool)      