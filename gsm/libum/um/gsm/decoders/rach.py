from um.tdma import Mapping, TypeAndOffset
from um.gsm.decoders.decoder import Decoder
from um.gsm.viterbi import Viterbi
from um.gsm.bitvector import Parity


MAPPING = Mapping( TypeAndOffset.TDMA_BEACON, \
                   False, \
                   True, \
                   1, \
                   True, \
                   51, \
                   # This comes from GSM 5.02 clause 7 table 5 of 9 page 46.
                   (4,5,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,45,46) )

class RACHDecoder(Decoder):
    def __init__( self ):
        Decoder.__init__( self, 'RACH', 0, 0, MAPPING )
        self.viterbi = Viterbi()
        self.parity  = Parity( 0x06f, 6, 8 )

    def on_burst( self, burst ):
        # The L1 FEC for the RACH is defined in GSM 05.03 4.6.
        e = burst.data.segment(49, 36)
        u = e.decode( self.viterbi, 18 )

        # To check validity, we have 4 tail bits and 6 parity bits.
        # False alarm rate for random inputs is 1/1024.

        # Check the tail bits -- should all the zero.
        if u.peek_field(14,4):
            return None

        d = u.segment(0,8)

        # Check the parity.
        # The parity word is XOR'd with the BSIC. (GSM 05.03 4.6.)
        sent_parity  = ~u.peek_field(8,6)
        check_parity = d.parity( self.parity )
        encoded_bsic = ( sent_parity ^ check_parity ) & 0x03f

        # We got a valid RACH burst.
        # The "payload" is an 8-bit field, "RA", defined in GSM 04.08 9.1.8.
        # The channel assignment procedure is in GSM 04.08 3.3.1.1.3.
        # It requires knowledge of the RA value and the burst receive time.
        # The RACH L2 is so thin that we don't even need code for it.
        d.lsb8msb()

        ra = d.peek_field(0,8)

        return ( encoded_bsic, ra )