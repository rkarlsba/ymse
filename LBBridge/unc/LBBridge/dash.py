# uncompyle6 version 3.8.0
# Python bytecode 3.7.0 (3394)
# Decompiled from: Python 3.7.3 (default, Jan 22 2021, 20:04:44) 
# [GCC 8.3.0]
# Embedded file name: /boot/LBBridge/LBBridge/dash.py
# Compiled at: 2021-09-13 16:26:38
# Size of source mod 2**32: 4716 bytes
import curses, time
from enum import Enum
from datetime import datetime
import sys
from . import proc_status
from .proc_status import STATUS
from . import version
HEADERS = {'time':'Current Time:', 
 'net':'Network:', 
 'relay':'Control Relay:', 
 'relay_info':'Relay Info:', 
 'cam':'Camera:', 
 'pi':'Raspberry Pi'}

class Dashboard:

    def __init__(self, scr, check_service=False):
        curses.noecho()
        curses.curs_set(0)
        self._Dashboard__start_color()
        if check_service:
            proc_status.BaseStatus.check_service_status = True
        self.scr = scr
        self.version = version.version_str()
        self.line = 0
        max_width = 0
        for val in HEADERS.values():
            if len(val) > max_width:
                max_width = len(val)

        self.title_min_width = 40
        self.header_width = max_width + 1
        self.indent = 2
        self.val_col = self.indent + self.header_width + 1
        self.title = f"LightBurn Bridge v{self.version}"
        if len(self.title) < self.title_min_width:
            rem = self.title_min_width - len(self.title)
            if rem % 2:
                rem += 1
            total_width = len(self.title) + rem
            self.title = self.title.ljust(len(self.title) + rem // 2)
            self.title = self.title.rjust(total_width)
        self.proc_wifi = proc_status.WiFiStatus()
        self.proc_relay = proc_status.RelayStatus()
        self.proc_relay_details = proc_status.RelayDetails()
        self.proc_pi = proc_status.PiStatus()

    def __start_color(self):
        curses.start_color()
        curses.init_pair(STATUS.none.value, curses.COLOR_WHITE, curses.COLOR_BLACK)
        curses.init_pair(STATUS.ok.value, curses.COLOR_GREEN, curses.COLOR_BLACK)
        curses.init_pair(STATUS.warn.value, curses.COLOR_YELLOW, curses.COLOR_BLACK)
        curses.init_pair(STATUS.error.value, curses.COLOR_RED, curses.COLOR_BLACK)

    def run(self):
        self.line = 0
        while True:
            self.update()
            time.sleep(1)

    def addline(self, line=''):
        self.scr.addstr(self.line, self.indent, line)
        self.line += 1

    def addheader(self, header, stat=STATUS.ok):
        self.scr.addstr(self.line, self.indent, header, curses.color_pair(stat.value) | curses.A_BOLD)

    def addval(self, val):
        self.scr.addstr(self.line, self.val_col, val)
        self.line += 1

    def update(self):
        self.line = 0
        self.scr.clear()
        self.add_title()
        self.addline()
        self.status_time()
        self.status_wifi()
        self.status_relay()
        self.status_relay_info()
        self.status_pi()
        self.scr.refresh()

    def add_title(self):
        self.addline(self.title)
        self.addline('*' * len(self.title))

    def status_time(self):
        now = datetime.now()
        msg = now.strftime('%H:%M:%S')
        status = STATUS.none
        header = HEADERS['time']
        self.addheader(header, status)
        self.addval(msg)

    def status_wifi(self):
        status, msg = self.proc_wifi.GetStatus()
        header = HEADERS['net']
        self.addheader(header, status)
        self.addval(msg)

    def status_relay(self):
        status, msg = self.proc_relay.GetStatus()
        header = HEADERS['relay']
        self.addheader(header, status)
        self.addval(msg)

    def status_relay_info(self):
        status, msg = self.proc_relay_details.GetStatus()
        header = HEADERS['relay_info']
        self.addheader(header, status)
        self.addval(msg)

    def status_camera(self):
        msg = 'No camera device found'
        status = STATUS.warn
        header = HEADERS['cam']
        self.addheader(header, status)
        self.addval(msg)

    def status_pi(self):
        status, msg = self.proc_pi.GetStatus()
        header = HEADERS['pi']
        if msg:
            self.addheader(header, status)
            self.addval(msg[0])
        else:
            self.addheader(header, STATUS.ok)
            self.addval('No issues found')


def main(scr):
    check_service = False
    if len(sys.argv) > 1:
        check_service = sys.argv[1] == '--cs'
    d = Dashboard(scr, check_service)
    try:
        d.run()
    except KeyboardInterrupt:
        curses.endwin()


def run():
    curses.wrapper(main)