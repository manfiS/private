import sys
import os

from um.trx.manager import Manager, CHANNEL_COMBINATION
from um.trx.channels.data import DataChannel

from um.gsm.decoders.rach import RACHDecoder


class TDMA:
    # number of time slots per frame
    N_SLOTS     = 8
    # maximum unified repeat period
    MAX_MODULUS = ( 51 * 26 * 4 )

    def __init__(self):
        self.demux_table = [[None] * self.MAX_MODULUS] * self.N_SLOTS

        self.install_decoder( RACHDecoder() )

    def install_decoder( self, decoder ):
        mapping = decoder.mapping
        for i in range( mapping.num_frames ):
            fn = mapping.frame_mapping(i)
            while fn < self.MAX_MODULUS:
                self.demux_table[ decoder.tn ][ fn ] = decoder
                fn += mapping.repeat_length

    def on_burst( self, burst ):
        fn      = burst.frame_number % self.MAX_MODULUS
        tn      = burst.timeslot_index
        decoder = self.demux_table[tn][fn]

        if decoder is not None:
            pkt = decoder.on_burst( burst )
            if pkt is not None:
                print pkt
            #else:
            #    print "fn=%d tn=%d - %s ( bad frame )" % (fn, tn, decoder.name)
        #else:
         #   print "fn=%d tn=%d - ???" % ( fn, tn )


mngr = Manager("trx/gsmtrx")
data = DataChannel( TDMA() )
data.start()

mngr.power_off()

mngr.tune( 945800, 930200 )
mngr.set_tsc( 2 )
mngr.set_max_delay( 4 )
mngr.set_rx_gain( 5 )
mngr.power_on()
mngr.set_power( 35 )

combinations = ( \
    CHANNEL_COMBINATION.V, # FCCH+SCH+CCCH+BCCH+SDCCH/4+SACCH/4, uplink RACH+SDCCH/4
    CHANNEL_COMBINATION.I, # TCH/FS
    CHANNEL_COMBINATION.I, # TCH/FS
    CHANNEL_COMBINATION.I, # TCH/FS
    CHANNEL_COMBINATION.I, # TCH/FS
    CHANNEL_COMBINATION.I, # TCH/FS
    CHANNEL_COMBINATION.I, # TCH/FS
    CHANNEL_COMBINATION.I  # TCH/FS
)

for tn, combination in enumerate(combinations):
    mngr.set_slot( tn, combination )
    mngr.clear_handover( tn )

mngr.set_power( 35 )

try:
    while True:
        data.join(600)
        if not data.isAlive():
            break
except KeyboardInterrupt:
    data.kill()
