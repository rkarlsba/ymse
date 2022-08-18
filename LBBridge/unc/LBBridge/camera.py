# uncompyle6 version 3.8.0
# Python bytecode 3.7.0 (3394)
# Decompiled from: Python 3.7.3 (default, Jan 22 2021, 20:04:44) 
# [GCC 8.3.0]
# Embedded file name: /boot/LBBridge/LBBridge/camera.py
# Compiled at: 2021-09-03 21:59:34
# Size of source mod 2**32: 5397 bytes
import cv2, socket, select, signal, threading, time
from . import log
from turbojpeg import TurboJPEG

class WebCam:

    def __init__(self, index=-1, thumb_scale=(1, 4), thumb_qual=70):
        self.index = index
        self.thumb_scale = thumb_scale
        self.thumb_qual = thumb_qual
        self.cap = cv2.VideoCapture(self.index)
        self.last_frame = None
        self.jpeg = TurboJPEG()
        log.info('Using MJPG mode')
        fourcc = (cv2.VideoWriter_fourcc)(*'MJPG')
        self.cap.set(cv2.CAP_PROP_FOURCC, fourcc)
        new_fourcc = self.cap.get(cv2.CAP_PROP_FOURCC)
        if new_fourcc != fourcc:
            raise Exception('Unable to switch to MJPG mode!')
        self.cap.set(cv2.CAP_PROP_CONVERT_RGB, 0.0)
        log.warning(self.cap.get(cv2.CAP_PROP_CONVERT_RGB))
        self.max_width, self.max_height = self._WebCam__setRes(120000, 120000)

    def __decode_fourcc(self, v):
        v = int(v)
        return ''.join([chr(v >> 8 * i & 255) for i in range(4)])

    def release(self):
        if self.cap:
            self.cap.release()

    def __setRes(self, width, height):
        self.cap.set(cv2.CAP_PROP_FRAME_WIDTH, width)
        self.cap.set(cv2.CAP_PROP_FRAME_HEIGHT, height)
        width = int(self.cap.get(cv2.CAP_PROP_FRAME_WIDTH))
        height = int(self.cap.get(cv2.CAP_PROP_FRAME_HEIGHT))
        return (width, height)

    def CaptureFull(self):
        result, frame = self.cap.read()
        self.last_frame = frame.tobytes()
        return (result, self.last_frame)

    def GetLastFull(self):
        return self.last_frame

    def CaptureThumb(self):
        result, frame = self.CaptureFull()
        if not result:
            return (False, None)
        try:
            frame = self.jpeg.scale_with_quality(frame, scaling_factor=(self.thumb_scale), quality=(self.thumb_qual))
        except IOError as ex:
            try:
                log.error(str(ex))
                return (False, None)
            finally:
                ex = None
                del ex

        return (
         result, frame)


class CameraServer:

    def __init__(self, cfg):
        self.cfg = cfg
        self.cam = WebCam()
        self.stop_lock = threading.Lock()
        self.stop = False
        self.server_ip = '0.0.0.0'
        self.server_port = 5006
        self.frame_time = 0.1
        signal.signal(signal.SIGINT, self.stop)
        signal.signal(signal.SIGTERM, self.stop)

    def stop(self):
        log.info('Camera server requested to stop')
        with self.stop_lock:
            self.stop = True

    def run(self):
        self.stop = False
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.bind((self.server_ip, self.server_port))
        log.info('Camera server started')
        sock.setblocking(0)
        sockets = [
         sock]
        last_thumb = 0.0
        self.cam.CaptureThumb()
        while 1:
            sock.listen(1)
            read, write, ex = select.select(sockets, sockets, sockets, 1.0)
            resp_packets = []
            if read:
                for insock in read:
                    try:
                        if insock is sock:
                            client, addr = insock.accept()
                            sockets.append(client)
                            log.info(f"Connection from {addr}")
                        else:
                            data = insock.recv(5)
                        if not data:
                            sockets.remove(insock)
                        else:
                            print(data)
                            if data[0] == ord('F'):
                                frame = self.cam.GetLastFull()
                                frame_size = len(frame)
                                resp = b'F' + frame_size.to_bytes(4, 'big')
                                resp += frame
                                resp_packets.append(resp)
                                print('Added full frame')
                    except ConnectionError:
                        pass

            if write:
                now = time.time()
                if now - last_thumb > self.frame_time:
                    res, frame = self.cam.CaptureThumb()
                    if res:
                        frame_size = len(frame)
                        resp = b'T' + frame_size.to_bytes(4, 'big')
                        resp += frame
                        resp_packets.append(resp)
                    last_thumb = now
                try:
                    for outsock in write:
                        for resp in resp_packets:
                            outsock.sendall(resp)
                            log.info(f"Sent {len(resp)} bytes")

                except ConnectionError:
                    pass