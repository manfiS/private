import socket

# Base class for all UDP transceiver channels.
class Channel:
    MAX_UDP_LENGTH = 1500

    def __init__( self, address, port ):
        self.server = ( address, port )
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, 0)
        self.socket.setsockopt( socket.SOL_SOCKET, socket.SO_REUSEADDR, 1 )
        self.socket.bind( ('', port + 100 ) )
