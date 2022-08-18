# uncompyle6 version 3.8.0
# Python bytecode 3.7.0 (3394)
# Decompiled from: Python 3.7.3 (default, Jan 22 2021, 20:04:44) 
# [GCC 8.3.0]
# Embedded file name: /boot/LBBridge/LBBridge/webdash.py
# Compiled at: 2021-09-14 13:18:28
# Size of source mod 2**32: 3265 bytes
from . import bottle
from . import util
from . import version
from .dash import HEADERS
from . import proc_status
from .proc_status import STATUS
from pathlib import Path
import sys

def FetchServiceLog(service):
    cmd = f'journalctl -n 2000 -u "{service}"'
    rc, output = util.shell(cmd, show_output=False)
    if rc != 0:
        return
    return output


class WebDash(bottle.Bottle):

    def __init__(self, check_service=False):
        super().__init__()
        if check_service:
            proc_status.BaseStatus.check_service_status = True
        self.base_path = Path(__file__).parent.absolute()
        self.static_path = self.base_path / 'webdash_static'
        self.template_path = self.base_path / 'webdash_template'
        bottle.TEMPLATE_PATH.append(str(self.template_path))
        self.version = version.version_str()
        self.title = f"LightBurn Bridge v{self.version}"
        self.proc_wifi = proc_status.WiFiStatus()
        self.proc_relay = proc_status.RelayStatus()
        self.proc_relay_details = proc_status.RelayDetails()
        self.proc_pi = proc_status.PiStatus()
        self._WebDash__setup_routes()

    def __setup_routes(self):
        self.route('/static/<filename>', callback=(self.static))
        self.route('/favicon.ico', callback=(self.favicon))
        self.route('/', callback=(self.index))
        self.route('/service_log/<service>', callback=(self.service_log))

    def make_header(self, status, header):
        result = f'<p class="{status.name}">{header}</p>'
        return result

    def static(self, filename):
        return bottle.static_file(filename, root=(self.static_path))

    def favicon(self):
        return self.static('favicon.ico')

    def index(self):
        args = {'title':self.title, 
         'status':[]}
        status, msg = self.proc_wifi.GetStatus()
        header = self.make_header(status, HEADERS['net'])
        args['status'].append((header, msg))
        status, msg = self.proc_relay.GetStatus()
        header = self.make_header(status, HEADERS['relay'])
        args['status'].append((header, msg))
        status, msg = self.proc_relay_details.GetStatus()
        header = self.make_header(status, HEADERS['relay_info'])
        args['status'].append((header, msg))
        status, msg = self.proc_pi.GetStatus()
        header = HEADERS['pi']
        if msg:
            msg = msg[0]
        else:
            status = STATUS.ok
            msg = 'No issues found'
        header = self.make_header(status, HEADERS['pi'])
        args['status'].append((header, msg))
        return (bottle.template)(*('index', ), **args)

    def service_log(self, service):
        output = FetchServiceLog(service)
        if output:
            bottle.response.set_header('Content-Type', 'text/plain')
            return output
        bottle.abort(500, 'Error retrieving service log')


def run():
    check_service = False
    if len(sys.argv) > 1:
        check_service = sys.argv[1] == '--cs'
    app = WebDash(check_service)
    app.run(host='0.0.0.0', port=80)