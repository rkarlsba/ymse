# uncompyle6 version 3.8.0
# Python bytecode 3.7.0 (3394)
# Decompiled from: Python 3.7.3 (default, Jan 22 2021, 20:04:44) 
# [GCC 8.3.0]
# Embedded file name: /boot/LBBridge/LBBridge/proc_status.py
# Compiled at: 2021-09-13 17:06:00
# Size of source mod 2**32: 4314 bytes
from enum import Enum
from .filelock import Timeout, FileLock
from pathlib import Path
import json
from . import log
from . import util
import time

class STATUS(Enum):
    none = 1
    ok = 2
    warn = 3
    error = 4


class BaseStatus:
    check_service_status = False

    def __init__(self, name):
        self.name = name
        self.base_dir = Path('/mnt/lbproc')
        self.status_file = self.base_dir / f"{self.name}.status"
        self.lock_file = self.base_dir / f"{self.name}.lock"
        self.service = None
        self.name = None
        self.is_json = False
        self.service_running = True
        self.service_check_interval = 15
        self.last_service_check = 0
        self.lock = FileLock(self.lock_file)

    def GetStatus(self):
        if BaseStatus.check_service_status:
            if self.service:
                if time.time() - self.last_service_check >= self.service_check_interval:
                    rc, _ = util.shell(f"systemctl is-active --quiet {self.service}")
                    self.service_running = rc == 0
                    self.last_service_check = time.time()
            else:
                return self.service_running or (
                 STATUS.error, f"{self.name} service is not running")
            status = STATUS.none
            res = ''
            if self.is_json:
                res = {}
        else:
            lines = []
            with self.lock:
                if not self.status_file.exists():
                    return (
                     status, res)
                with open(self.status_file, 'r') as (f):
                    lines = f.readlines()
                    if len(lines) != 2:
                        return (
                         status, res)
            status_str = lines[0].strip()
            try:
                status_int = int(status_str)
                status = STATUS(status_int)
            except ValueError:
                log.error(f"{self.name} status: error parsing status val f{status_str}")

            res = lines[1].rstrip()
            if self.is_json:
                try:
                    res = json.loads(res)
                except:
                    log.error(f"{self.name} status: error parsing status val f{res}")
                    res = {}

        return (
         status, res)

    def SetStatus(self, status, msg):
        if self.is_json:
            msg = json.dumps(msg)
        with self.lock:
            with open(self.status_file, 'w') as (f):
                f.write(f"{status.value}\n")
                f.write(msg)

    def info(self, msg):
        log.info(msg)
        self.SetStatus(STATUS.none, msg)

    def ok(self, msg):
        log.info(msg)
        self.SetStatus(STATUS.ok, msg)

    def warn(self, msg):
        log.warning(msg)
        self.SetStatus(STATUS.warn, msg)

    def error(self, msg):
        log.error(msg)
        self.SetStatus(STATUS.error, msg)


class WiFiStatus(BaseStatus):

    def __init__(self):
        super().__init__('wifi')
        self.service = 'lbnetwork'
        self.name = 'Network'

    def SetConnectedState(self, ip=None):
        if ip:
            self.ok(f"Connected -> {ip}")
        else:
            self.error('Disconnected')

    def SetAPMode(self, name):
        self.warn(f"WiFi Config AP running: {name}")

    def SetConnecting(self, ap_name=None):
        if ap_name:
            self.warn(f"Attempting to connect to {ap_name}")
        else:
            self.warn('Attempting to connect...')


class RelayStatus(BaseStatus):

    def __init__(self):
        super().__init__('relay')
        self.service = 'lbrelay'
        self.name = 'Relay'


class RelayDetails(BaseStatus):

    def __init__(self):
        super().__init__('relay_details')
        self.service = 'lbrelay'
        self.name = 'Relay'


class PiStatus(BaseStatus):

    def __init__(self):
        super().__init__('pi')
        self.is_json = True
        self.service = 'lbpistatus'
        self.name = 'Pi Status'

    def GetStatus(self):
        status, msg = super().GetStatus()
        if isinstance(msg, str):
            msg = [
             msg]
        else:
            if msg == {}:
                msg = []
        return (
         status, msg)