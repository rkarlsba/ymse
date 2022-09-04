# uncompyle6 version 3.8.0
# Python bytecode 3.7.0 (3394)
# Decompiled from: Python 3.7.3 (default, Jan 22 2021, 20:04:44) 
# [GCC 8.3.0]
# Embedded file name: /boot/LBBridge/LBBridge/pi_state.py
# Compiled at: 2021-09-13 17:05:24
# Size of source mod 2**32: 3339 bytes
import subprocess, threading, signal
from time import sleep
from . import log
from .proc_status import PiStatus, STATUS
from .filelock import Timeout, FileLock
THROTTLE_MESSAGES = {0:(True, 'Under-voltage!'), 
 1:(True, 'ARM frequency capped!'), 
 2:(True, 'Currently throttled!'), 
 3:(True, 'Soft temperature limit active!'), 
 16:(False, 'Under-voltage has occurred since last reboot.'), 
 17:(False, 'Throttling has occurred since last reboot.'), 
 18:(False, 'ARM frequency capping has occurred since last reboot.'), 
 19:(False, 'Soft temperature limit has occurred since last reboot')}

class PiState:

    def __init__(self):
        self._PiState__is_pi = False
        try:
            import RPi.GPIO as gpio
            self._PiState__is_pi = True
        except:
            pass

        self.stop_lock = threading.Lock()
        self.stop = False
        signal.signal(signal.SIGINT, self.stop)
        signal.signal(signal.SIGTERM, self.stop)
        self.throttle_cmd = 'vcgencmd get_throttled'
        self.status = PiStatus()
        self.state = []

    def stop(self):
        with self.stop_lock:
            self.stop = True

    def IsPi(self):
        return self._PiState__is_pi

    def __get_throttled(self):
        throttled_output = subprocess.check_output((self.throttle_cmd), shell=True)
        throttled_output = throttled_output.strip()
        hex_val = throttled_output.split(b'=')[1]
        throttled_binary = int(hex_val, base=16)
        msgs = []
        for position, message in THROTTLE_MESSAGES.items():
            if throttled_binary & 1 << position > 0:
                msgs.append(message)

        return (
         bool(len(msgs)), msgs)

    def UpdateState(self):
        status_type = STATUS.ok
        if not self._PiState__is_pi:
            return status_type
        status, msgs = self._PiState__get_throttled()
        if not status:
            self.state = []
            log.info('No Pi power issues')
        else:
            is_error = False
            for error, msg in msgs:
                if error:
                    log.error(msg)
                    is_error = True
                else:
                    log.warning(msg)

            self.state = msgs
            if is_error:
                status_type = STATUS.error
            else:
                status_type = STATUS.warn
        self.status.SetStatus(status_type, msgs)
        return status_type

    def run(self):
        while True:
            status = self.UpdateState()
            with self.stop_lock:
                if self.stop:
                    break
            if status == STATUS.ok:
                sleep(180)
            else:
                sleep(30)