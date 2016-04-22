from um.gsm.softvector import SoftVector
from struct import unpack

# Basic incoming burst from the transceiver.
class Burst:
    SLOT_LENGTH = 148

    def __init__( self, data ):
        self.timeslot_index   = ord(data[0])
        self.frame_number     = unpack( '>I', data[1:5] )[0]
        self.rssi_attenuation = -ord(data[5])
        self.timing_error     = unpack( '>h', data[6:8] )[0] / 256.0
        # remove guard values
        self.data             = SoftVector( data[8:] )
