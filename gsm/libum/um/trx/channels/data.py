import socket
import threading
import traceback

from um.gsm.burst import Burst
from um.trx.channels.channel import Channel


# Bursts I/O channel.
class DataChannel(threading.Thread, Channel):
    def __init__( self, scheduler, address = '127.0.0.1', port = 5702 ):
        Channel.__init__( self, address, port )
        threading.Thread.__init__(self)
        self.scheduler = scheduler
        self.running = threading.Event()
        self.running.set()

    def kill(self):
        self.running.clear()
        try:
            self.socket.shutdown(socket.SHUT_RDWR)
            self.socket.close()
        except:
            pass
        self.join()

    def run(self):
        """
        Received Data Burst

            1 byte timeslot index
            4 bytes GSM frame number, big endian
            1 byte RSSI in -dBm
            2 bytes correlator timing offset in 1/256 symbol steps, 2's-comp, big endian
            148 bytes soft symbol estimates, 0 -> definite "0", 255 -> definite "1"
        """
        while self.running.is_set():
            try:
                data  = self.socket.recv(self.MAX_UDP_LENGTH)
                burst = Burst( data )
                self.scheduler.on_burst( burst )
            except Exception as e:
                print(traceback.format_exc())

