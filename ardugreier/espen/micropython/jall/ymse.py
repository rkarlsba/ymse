def getonthenetwork():
    import network
    sta_if = network.WLAN(network.STA_IF)
    if not sta_if.isconnected():
        print('connecting to network...')
        sta_if.active(True)
        sta_if.connect('HelseSorOst', '')
        while not sta_if.isconnected():
            pass
    print('network config:', sta_if.ifconfig())

def getsystemp(unit):
    import esp32
    tf = esp32.raw_temperature()
    tc = (tf-32.0)/1.8

    if (unit == "F"):
        return tf
    elif (unit == "C"):
        return tc
    elif (unit == "K"):
        return tc+273.15
    elif (unit == "R"):
        return tf+459.67
    else
        return -4242424242424242

def systemp(unit="C"):
    temp = getsystemp(unit)
    print("Current temperature is {1:5.1f} degrees {}".format(temp,unit)

#   print("T = {0:4d} deg F or {1:5.1f}  deg C".format(tf,tc))
#   print("{1:5.1f} deg C".format(tc))
