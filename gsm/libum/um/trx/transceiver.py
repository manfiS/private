import os
import threading
import subprocess

class Transceiver(threading.Thread):
    def __init__( self, path ):
        threading.Thread.__init__(self)
        self.path = os.path.abspath( path )
        self.proc = None

        if not os.path.isfile(self.path):
            raise Exception( "Transceiver '%s' does not exist." % self.path )

    def __del__(self):
        self.proc.kill()

    def run(self):
        self.proc = subprocess.Popen( "%s" % self.path )
