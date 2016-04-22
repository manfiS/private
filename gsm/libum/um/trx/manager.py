from um.trx.channels.control import ControlChannel
from um.trx.transceiver import Transceiver

import time

class CHANNEL_COMBINATION:
    FILL     = 0  # Channel is transmitted, but unused
    I        = 1  # TCH/FS
    II       = 2  # TCH/HS, idle every other slot
    III      = 3  # TCH/HS
    IV       = 4  # FCCH+SCH+CCCH+BCCH, uplink RACH
    V        = 5  # FCCH+SCH+CCCH+BCCH+SDCCH/4+SACCH/4, uplink RACH+SDCCH/4
    VI       = 6  # CCCH+BCCH, uplink RACH
    VII      = 7  # SDCCH/8 + SACCH/8
    NONE     = 8  # Channel is inactive, default
    LOOPBACK = 9  # similar go VII, used in loopback testing
    IGPRS	 = 1  # GPRS channel, like I but static filler frames.

class Manager(ControlChannel):
    def __init__( self, trxpath, address = '127.0.0.1', port = 5701 ):
        ControlChannel.__init__(self, address, port)
        self.trx = Transceiver(trxpath)
        self.trx.start()
        time.sleep(1)

    def power_off( self ):
        self.send_command('POWEROFF')

    def power_on( self ):
        self.send_command('POWERON')

    def get_factory_calibration( self, param ):
        return int( self.send_command( "READFACTORY %s" % param ) )

    def tune( self, rxfreq, txfreq ):
        self.send_command( "RXTUNE %d" % rxfreq )
        self.send_command( "TXTUNE %d" % txfreq )

    def set_tsc( self, tsc ):
        """
        GSM basestation color code; lower 3 bits of the BSIC.
        BCC/TSC values in a multi-BTS network should be assigned so that BTS
        units with overlapping coverage do not share a BCC.
        This value will also select the training sequence used for all slots
        on this unit.
        """
        self.send_command( "SETTSC %d" % tsc )

    def set_bsic( self, bsic ):
        """
        BSIC = ( NCC << 3) | BCC
        GSM network color code; upper 3 bits of the BSIC.  Assigned by your national regulator.  Must be distinct from NCCs of other GSM operators in your area.
        """
        self.send_command( "SETBSIC %d" % bsic )

    def set_max_delay( self, km ):
        """
        1 GSM symbol is approx. 1 km
        """
        self.send_command( "SETMAXDLY %d"  % km )

    def set_rx_gain( self, rxgain ):
        self.send_command( "SETRXGAIN %d" % rxgain )

    def set_power( self, db ):
        self.send_command( "SETPOWER %d" % db )

    def set_slot( self, tn, combination ):
        self.send_command( "SETSLOT %d %d" % ( tn, combination ) )

    def set_handover( self, tn ):
        self.send_command( "HANDOVER %d" % tn )

    def clear_handover( self, tn ):
        self.send_command( "NOHANDOVER %d" % tn )

    def set_tx_atten( self, txatten ):
        self.send_command( "SETTXATTEN %d" % txatten )

    def set_freq_offset( self, offset ):
        self.send_command( "SETFREQOFFSET %d" % offset )

    def get_noise_level( self ):
        return int( self.send_command( "NOISELEV 0" ) )
