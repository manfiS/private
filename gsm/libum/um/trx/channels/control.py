from um.trx.errors import ResponseError, UnexpectedResponseError
from um.trx.channels.channel import Channel
import re

# Control channel.
class ControlChannel(Channel):
    DEBUG = False

    def __init__( self, address, port ):
        Channel.__init__( self, address, port )

    def __debug( self, msg ):
        if self.DEBUG:
            print msg

    def __parse_response( self, command, data ):
        command = command.split(' ')[0]
        m = re.search( "^RSP %s (\d+)\s*(.*)$" % command, data )
        if m:
            status   = m.group(1)
            response = m.group(2)

            if int(status) != 0:
                raise ResponseError( "%s failed with status %s." % (command, status) )
            else:
                return response.strip('\0')
        else:
            raise UnexpectedResponseError( "Unexpected response '%s' for command %s." % ( data, command ) )

    def send_command( self, command ):
        pkt = "CMD %s" % command
        self.__debug( ">> %s" % pkt )
        self.socket.sendto( pkt, self.server )
        data = self.socket.recv(self.MAX_UDP_LENGTH)
        self.__debug( "<< %s" % data )
        return self.__parse_response( command, data )
