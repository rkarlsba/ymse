from machine import ADC, Pin
import time

# "Constants"
PROBE_PIN = 36

# Modes
MODE_MEASURE = 1
MODE_CONTINUOUS_PERCENTAGE = 2
MODE_CONTINUOUS_VALUES = 3
MODE_CONTINUOUS_BOTH = 4

# Measurements
MEASURE_COUNT = 100
MEASURE_DELAY = 0.1  # 100 ms

# Kalibrering
DRY = 2500
WET = 900

# Pin setup
adc = ADC(Pin(PROBE_PIN))
adc.atten(ADC.ATTN_11DB)

def to_percent(value):
    moisture = (DRY - value) / (DRY - WET)
    moisture = max(0, min(1, moisture))  # clamp 0–1
    return moisture * 100

def median(lst):
    s = sorted(lst)
    n = len(s)
    if n % 2 == 0:
        return (s[n//2 - 1] + s[n//2]) / 2
    else:
        return s[n//2]

# Mode choice
# mode = MODE_MEASURE
mode = MODE_CONTINUOUS_PERCENTAGE
# mode = MODE_CONTINUOUS_VALUES
# mode = MODE_CONTINUOUS_BOTH

if mode == MODE_MEASURE:
    samples = []

    print("Starter måling...")

    for i in range(MEASURE_COUNT):
        samples.append(adc.read())
        time.sleep(MEASURE_DELAY)

    print("Målinger ferdig")

    avg = sum(samples) / len(samples)
    med = median(samples)

    print("Gjennomsnitt:", avg)
    print("Median:", med)
    print("Fukt (%), snitt:", to_percent(avg))
    print("Fukt (%), median:", to_percent(med))
elif mode == MODE_CONTINUOUS_PERCENTAGE or mode == MODE_CONTINUOUS_VALUES or mode == MODE_CONTINUOUS_BOTH:
    while True:
        # ta noen samples og bruk median (stabilt signal)
        samples = [adc.read() for _ in range(5)]
        med = median(samples)
        percent = to_percent(med)

        if mode == MODE_CONTINUOUS_PERCENTAGE:
            print(percent)
        elif mode == MODE_CONTINUOUS_VALUES:
            print(med)
        elif mode == MODE_CONTINUOUS_BOTH:
            print(med, percent)
                
        time.sleep(MEASURE_DELAY)

else:
    print("Unknown mode", mode)

# Ende
