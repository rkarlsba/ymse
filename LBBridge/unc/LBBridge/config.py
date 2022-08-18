# uncompyle6 version 3.8.0
# Python bytecode 3.7.0 (3394)
# Decompiled from: Python 3.7.3 (default, Jan 22 2021, 20:04:44) 
# [GCC 8.3.0]
# Embedded file name: /boot/LBBridge/LBBridge/config.py
# Compiled at: 2021-08-27 12:38:50
# Size of source mod 2**32: 1873 bytes
import json
from pathlib import Path
import uuid
from . import log
CONFIG_DEFAULTS = {'device_type':'ruida', 
 'laser_ip':'10.0.3.3', 
 'ap_name':None}

class Config:

    def __init__(self):
        self._Config__path = Path('/boot/bridge.json')
        self._Config__reset()
        self.Load()

    def __reset(self):
        for key, val in CONFIG_DEFAULTS.items():
            self.__dict__[key] = val

    def Load(self):
        self._Config__reset()
        data = {}
        load_fail = False
        if not self._Config__path.exists():
            log.warning(f"{self._Config__path} does not exist! Setting to defaults.")
        else:
            with open(self._Config__path, 'r') as (f):
                try:
                    data = json.load(f)
                except:
                    load_fail = True
                    data = {}

        for key, val in CONFIG_DEFAULTS.items():
            self.__dict__[key] = data.get(key, self.__dict__[key])

        if self.ap_name is None:
            suffix = str(uuid.uuid4())[:4].upper()
            self.ap_name = f"LightBurn Bridge {suffix}"
            if not load_fail:
                self.Save()

    def Save(self):
        data = {}
        for key in CONFIG_DEFAULTS.keys():
            data[key] = self.__dict__[key]

        with open(self._Config__path, 'w') as (f):
            json.dump(data, f, indent=2)

    def to_dict(self):
        result = {}
        for key, val in CONFIG_DEFAULTS.items():
            result[key] = self.__dict__[key]

        return result