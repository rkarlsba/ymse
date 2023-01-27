def systemp():
    import esp32
    tf = esp32.raw_temperature()
    tc = (tf-32.0)/1.8
#   print("T = {0:4d} deg F or {1:5.1f}  deg C".format(tf,tc))
    print("{1:5.1f} deg C".format(tc))
