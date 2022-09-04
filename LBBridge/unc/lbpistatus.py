# uncompyle6 version 3.8.0
# Python bytecode 3.7.0 (3394)
# Decompiled from: Python 3.7.3 (default, Jan 22 2021, 20:04:44) 
# [GCC 8.3.0]
# Embedded file name: /boot/LBBridge/lbpistatus.py
# Compiled at: 2021-09-13 16:26:38
# Size of source mod 2**32: 177 bytes
from LBBridge import log
log.init_logger('lbpistatus')
from LBBridge import pi_state

def main():
    ps = pi_state.PiState()
    ps.run()


if __name__ == '__main__':
    main()