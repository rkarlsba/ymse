from machine import ADC, Pin
import time

# "Constants"
PROBE_PIN = 36
MODE_CONTINUOUS = 1
MODE_MEASURE = 2
MEASURE_COUNT = 100
MEASURE_DELAY = 0.1 # 100ms

# Pin setup
adc = ADC(Pin(PROBE_PIN))
adc.atten(ADC.ATTN_11DB)

# Mode choice
mode = MODE_CONTINUOUS
mode = MODE_MEASURE

if mode == MODE_MEASURE:
    samples = []

    print("Starter måling...")

    for i in range(100):
        value = adc.read()
        samples.append(value)
        time.sleep(0.1)  # 100 ms

    print("Målinger ferdig")

    # --- Gjennomsnitt ---
    avg = sum(samples) / len(samples)

    # --- Median ---
    sorted_samples = sorted(samples)
    n = len(sorted_samples)

    if n % 2 == 0:
        median = (sorted_samples[n//2 - 1] + sorted_samples[n//2]) / 2
    else:
        median = sorted_samples[n//2]

    print("Gjennomsnitt:", avg)
    print("Median:", median)

elif mode == MODE_CONTINUOUS:
    while True:
        print(adc.read())
        time.sleep(0.1)
        
else:
    print(f"Unknown mode {mode}")
    
