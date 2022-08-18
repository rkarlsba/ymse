# uncompyle6 version 3.8.0
# Python bytecode 3.7.0 (3394)
# Decompiled from: Python 3.7.3 (default, Jan 22 2021, 20:04:44) 
# [GCC 8.3.0]
# Embedded file name: /boot/LBBridge/LBBridge/version.py
# Compiled at: 2021-09-13 16:26:38
# Size of source mod 2**32: 193 bytes
__version__ = (1, 0, 0, 'RC3')

def version_str():
    v = [str(i) for i in __version__]
    res = '.'.join(v[0:3])
    if len(v) > 3:
        if v[3]:
            res += f"-{v[3]}"
    return res