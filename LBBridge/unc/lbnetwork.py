# uncompyle6 version 3.8.0
# Python bytecode 3.7.0 (3394)
# Decompiled from: Python 3.7.3 (default, Jan 22 2021, 20:04:44) 
# [GCC 8.3.0]
# Embedded file name: /boot/LBBridge/lbnetwork.py
# Compiled at: 2021-09-14 13:32:20
# Size of source mod 2**32: 2372 bytes
from LBBridge import log
from LBBridge import config
log.init_logger('lbnetwork')
from LBBridge import network
from LBBridge.proc_status import WiFiStatus
from time import sleep
from pathlib import Path
CHECK_INTERVAL = 20
AP_TIMEOUT = 300
nm = network.NM()
ws = WiFiStatus()

def check_for_wifi():
    log.info('Checking wifi...')
    if not nm.HasWiFi():
        sleep(2)
        for _ in range(5):
            if nm.HasWiFi():
                return True
            sleep(2)

        return False
    return True


def check_for_wifi_config():
    ws.info('Checking for /boot/wifi.txt')
    config_path = Path('/boot/wifi.txt')
    if config_path.exists():
        with open(config_path, 'r') as (f):
            lines = f.readlines()
        if len(lines) > 2:
            lines = lines[:2]
        ssid = None
        password = None
        for l in lines:
            l = l.strip()
            split = l.split('=')
            if len(split) == 2:
                if split[0].strip().lower() == 'ssid':
                    ssid = split[1].strip()
                elif split[0].strip().lower() in ('pass', 'password'):
                    log.info('Set pass')
                    password = split[1].strip()

        if ssid:
            ws.info(f"Connecting to {ssid}")
            ws.SetConnecting(ssid)
            if nm.WiFiConnectToNetwork(ssid, password):
                log.info('WiFi Connect Success!')
                log.info('Moving /boot/wifi.txt to /boot/wifi.txt.loaded')
                config_path.rename('/boot/wifi.txt.loaded')
            else:
                log.info('Moving /boot/wifi.txt to /boot/wifi.txt.failed')
                config_path.rename('/boot/wifi.txt.failed')


def main():
    cfg = config.Config()
    check_for_wifi_config()
    ws.info('Checking for WiFi connection...')
    while True:
        if not check_for_wifi():
            log.info('WiFi Down. Starting AP...')
            ws.SetAPMode(cfg.ap_name)
            nm.StartWiFiConnect(cfg.ap_name, AP_TIMEOUT)
            sleep(5)
        log.info('WiFi Connected')
        ip = None
        if nm.HasWiFi():
            ip = nm.WiFiIP()
        ws.SetConnectedState(ip)
        sleep(CHECK_INTERVAL)


if __name__ == '__main__':
    main()