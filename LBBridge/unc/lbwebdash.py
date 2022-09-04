# uncompyle6 version 3.8.0
# Python bytecode 3.7.0 (3394)
# Decompiled from: Python 3.7.3 (default, Jan 22 2021, 20:04:44) 
# [GCC 8.3.0]
# Embedded file name: /boot/LBBridge/lbwebdash.py
# Compiled at: 2021-09-13 16:26:38
# Size of source mod 2**32: 128 bytes
from LBBridge import log
log.init_logger('lbwebdash')
from LBBridge import webdash
if __name__ == '__main__':
    webdash.run()