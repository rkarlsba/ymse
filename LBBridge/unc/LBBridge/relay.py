# uncompyle6 version 3.8.0
# Python bytecode 3.7.0 (3394)
# Decompiled from: Python 3.7.3 (default, Jan 22 2021, 20:04:44) 
# [GCC 8.3.0]
# Embedded file name: /boot/LBBridge/LBBridge/relay.py
# Compiled at: 2021-09-13 16:26:38
# Size of source mod 2**32: 7033 bytes
import socket, select, signal, threading, time
from enum import Enum
from . import log

class RelayType(Enum):
    Ethernet = 1
    Serial = 2


class Relay:

    def __init__(self, cfg, status):
        self.version = (0, 0)
        self.cfg = cfg
        self.status = status
        self.stop_lock = threading.Lock()
        self.stop = False
        self.type = RelayType.Ethernet
        self.laser_ip = self.cfg.laser_ip
        self.server_ip = '0.0.0.0'
        self.server_port = 5005
        signal.signal(signal.SIGINT, self.stop)
        signal.signal(signal.SIGTERM, self.stop)

    def run(self):
        log.error('Cannot call run() on base Relay class')

    def stop(self):
        log.info('Relay service requested to stop')
        with self.stop_lock:
            self.stop = True


class RuidaRelay(Relay):

    def __init__(self, cfg, status):
        super().__init__(cfg, status)
        self.version = (1, 0)
        self.type = RelayType.Ethernet
        self.buffer_size = 1024
        self.from_laser_port = 40200
        self.to_laser_port = 50200

    def runConnection(self, sock):
        outSock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        inSock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        inSock.bind(('', self.from_laser_port))
        inSock.setblocking(0)
        serv, addr = sock.accept()
        self.status.ok(f"Connection from: {addr[0]}:{addr[1]}")
        serv.setsockopt(socket.SOL_TCP, socket.TCP_NODELAY, 1)
        serv.setsockopt(socket.SOL_SOCKET, socket.SO_SNDBUF, 4096)
        serv.setsockopt(socket.SOL_SOCKET, socket.SO_RCVBUF, 32768)
        packet = b''
        packetLen = 0
        lastLen = 0
        packetType = ord('L')
        ackValue = b''
        gotAck = True
        lastTime = time.time()
        while 1:
            readable, writable, exceptional = select.select([inSock, serv], [], [serv], 1.0)
            if serv in exceptional:
                self.status.error('Server socket error')
                break
            if gotAck:
                if serv in readable:
                    try:
                        if packetLen == 0:
                            data = serv.recv(3 - len(packet))
                        else:
                            data = serv.recv(packetLen - len(packet))
                    except Exception:
                        pass

                    if not data:
                        break
                    packet += data
                    if packetLen == 0:
                        if len(packet) == 3:
                            packetType = packet[0]
                            packetLen = (packet[1] << 8) + packet[2]
                            packet = serv.recv(packetLen)
                    if packetLen != 0:
                        if len(packet) == packetLen:
                            if packetType == ord('L'):
                                outSock.sendto(packet, (self.laser_ip, self.to_laser_port))
                                lastLen = packetLen
                                packetLen = 0
                                packet = b''
                                lastTime = time.time()
                                gotAck = False
                            else:
                                if packetType == ord('P'):
                                    data = b'P\x00\x02' + bytes(self.version)
                                    serv.send(data)
                                else:
                                    self.status.error(f"unhandled packet type {packetType}" + str(bytes(chr(packetType))))
            if inSock in readable:
                data, addr = inSock.recvfrom(self.buffer_size)
                if len(data) > 1 or lastLen <= 500:
                    hdr = bytes([packetType, len(data) >> 8, len(data) & 255])
                    serv.send(hdr)
                    serv.send(data)
                if len(data) == 1:
                    if len(ackValue) == 0:
                        ackValue = data
                        gotAck = True
                    else:
                        if ackValue[0] != data[0]:
                            self.status.warn('Non-ack received')
                            break
                        else:
                            gotAck = True
            if gotAck == False and time.time() - lastTime > 6:
                self.status.error('Laser timeout error')
                break

        serv.close()
        outSock.close()
        inSock.close()
        self.status.ok('Ruida command complete')

    def run(self):
        self.stop = False
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.bind((self.server_ip, self.server_port))
        self.status.ok(f"Ruida relay started, laser IP: {self.laser_ip}")
        sock.setblocking(0)
        while True:
            sock.listen(1)
            gotConnect = False
            while True:
                readable, writable, exceptional = select.select([sock], [], [sock], 1.0)
                if sock in readable:
                    gotConnect = True
                    break
                if sock in exceptional:
                    break
                with self.stop_lock:
                    if self.stop:
                        break

            if gotConnect == False:
                break
            self.runConnection(sock)
            with self.stop_lock:
                if self.stop:
                    break

        self.status.info('Ruida relay stopped')


RELAYS = {'ruida': RuidaRelay}