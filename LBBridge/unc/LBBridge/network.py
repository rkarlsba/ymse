# uncompyle6 version 3.8.0
# Python bytecode 3.7.0 (3394)
# Decompiled from: Python 3.7.3 (default, Jan 22 2021, 20:04:44) 
# [GCC 8.3.0]
# Embedded file name: /boot/LBBridge/LBBridge/network.py
# Compiled at: 2021-09-14 13:18:28
# Size of source mod 2**32: 5744 bytes
from . import util
from . import log
from time import sleep
import ipaddress

class NM:

    def __init__(self):
        self.ssid = None

    def GetInterfaceDetails(self, iface):
        fields = {'Connection':'GENERAL.CONNECTION', 
         'IP':'IP4.ADDRESS'}
        result = {}
        for key, field in fields.items():
            rc, output = util.shell(f"nmcli -g {field} device show {iface}")
            if rc != 0:
                result[key] = None
            else:
                val = output.strip()
                if val:
                    result[key] = val
                else:
                    result[key] = None

        if result['IP']:
            ip = result['IP'].split('/')[0]
            result['IP'] = ip
        return result

    def GetWiFiDetails(self):
        return self.GetInterfaceDetails('wlan0')

    def GetEthernetDetails(self):
        return self.GetInterfaceDetails('eth0')

    def HasWiFi(self):
        status, output = util.shell('iwgetid -r')
        if status != 0:
            self.ssid = None
            return False
        self.ssid = output.strip()
        return True

    def WiFiIP(self):
        details = self.GetWiFiDetails()
        return details['IP']

    def WiFiOn(self):
        rc, _ = util.shell('nmcli radio wifi on')
        return rc == 0

    def WiFiDown(self):
        rc, _ = util.shell('nmcli device disconnect wlan0')
        return rc == 0

    def WiFiUp(self):
        rc, _ = util.shell('nmcli device connect wlan0')
        return rc == 0

    def WiFiConnectToNetwork(self, ssid, password=None):
        self.WiFiOn()
        self.WiFiDown()
        log.info('Wait for WiFi state to stabilize...')
        sleep(5)
        log.info(f"{ssid}:{password}")
        cmd = f"nmcli dev wifi connect {ssid}"
        if password is not None:
            cmd += f' password "{password}"'
        rc, output = util.shell(cmd)
        if output.strip().startswith('Error'):
            return False
        return rc == 0

    def WiFiClearNetworks(self):
        self.WiFiDown()
        cmd = 'nmcli -f uuid,type con show'
        rc, output = util.shell(cmd)
        if rc == 0:
            lines = output.split('\n')
            for l in lines:
                if 'wifi' in l:
                    uuid = l.split(' ')[0]
                    uuid = uuid.strip()
                    cmd = f"nmcli con delete {uuid}"
                    util.shell(cmd)

    def ResetWiFiConnect(self):
        util.shell('wifi-connect -a 1')

    def StartWiFiConnect(self, ssid=None, timeout=300):
        self.WiFiOn()
        self.WiFiDown()
        util.shell('systemctl stop lbwebdash')
        cmd = 'wifi-connect'
        if ssid:
            cmd += f' -s "{ssid}"'
        if timeout:
            cmd += f" -a {timeout}"
        try:
            try:
                status, _ = util.shell(cmd, show_output=True)
            except:
                self.ResetWiFiConnect()

        finally:
            sleep(2)
            self.WiFiUp()
            util.shell('systemctl start lbwebdash')

    def EthGetDirectConIP(self):
        cmd = 'nmcli -g ipv4.addresses con show LBBDirect'
        rc, output = util.shell(cmd)
        output = output.strip()
        if rc != 0 or output.startswith('Error'):
            return
        lines = output.split('\n')
        if lines:
            full_ip = lines[0]
            ip_split = full_ip.split('/')
            if ip_split:
                return ip_split[0]

    def EthKillDirect(self):
        util.shell('nmcli connection delete id LBBDirect')
        util.shell('nmcli device connect eth0')

    def EthConfigDirect(self, ip, is_dest=False):
        try:
            ipaddress.ip_address(ip)
        except ValueError:
            log.show_user(f"{ip} is not a valid IP address. Cannot configure ethernet")
            return False
        else:
            final_ip = ip
            ip_split = ip.split('.')
            if is_dest:
                if ip_split[3] == '1':
                    ip_split[3] = '2'
                else:
                    ip_split[3] = '1'
                final_ip = '.'.join(ip_split)
            wifi_ip = self.WiFiIP()
            if wifi_ip:
                wifi_split = wifi_ip.split('.')
                if wifi_split[:3] == ip_split[:3]:
                    log.show_user('Laser controller cannot be configured with an IP on the same subnet as your WiFi network. Please choose another in /boot/bridge.json')
                    return False
            cur_ip = self.EthGetDirectConIP()
            if cur_ip:
                if cur_ip == final_ip:
                    log.info(f"Requested eth0 IP of {final_ip} is already configured. Skipping setup.")
                    return True
            self.EthKillDirect()
            sleep(2)
            util.shell('sudo nmcli con down LBBDirect')
            cmd = f"nmcli c add type ethernet ifname eth0 con-name LBBDirect ip4 {final_ip}/24"
            rc, _ = util.shell(cmd)
            if rc == 0:
                util.shell('sudo nmcli con up LBBDirect')
                return True
            return False