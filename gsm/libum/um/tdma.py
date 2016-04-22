# Type and TDMA offset of a logical channel, from GSM 04.08 10.5.2.5
class TypeAndOffset:
    TDMA_MISC = 0
    TCHF_0    = 1
    TCHH_0    = 2
    TCHH_1    = 3
    SDCCH_4_0 = 4
    SDCCH_4_1 = 5
    SDCCH_4_2 = 6
    SDCCH_4_3 = 7
    SDCCH_8_0 = 8
    SDCCH_8_1 = 9
    SDCCH_8_2 = 10
    SDCCH_8_3 = 11
    SDCCH_8_4 = 12
    SDCCH_8_5 = 13
    SDCCH_8_6 = 14
    SDCCH_8_7 = 15
    # Some extra ones for our internal use.
    TDMA_BEACON_BCCH = 253
    TDMA_BEACON_CCCH = 252
    TDMA_BEACON      = 255
    TDMA_PDCH        = 256 # packet data channel, inclusive
    TDMA_PACCH       = 257 # packet control channel, shared with data but distinguished in MAC header.
    TDMA_PTCCH       = 258 # packet data timing advance logical channel
    TDMA_PDIDLE		 = 259 # Handles the packet channel idle frames.

class Mapping:
    # The longest "repeat length" of any channel we support is 104 for the SACCH/TF.
    MAX_REPEAT_LENGTH  = 104

    # Construct a TDMA mapping, encoding one line of GSM 05.02 Clause 7 Tables 1-4.
    def __init__( self, type_and_offset, downlink, uplink, allowed_slots, c0_only, repeat_length, frame_mapping ):
        self.type_and_offset = type_and_offset
        self.downlink        = downlink
        self.uplink          = uplink
        self.allowed_slots   = allowed_slots
        self.c0_only         = c0_only
        self.repeat_length   = repeat_length
        self.num_frames      = len(frame_mapping)

        self.__frame_mapping   = frame_mapping
        self.__revere_mapping  = [-1] * Mapping.MAX_REPEAT_LENGTH

        for i in range( self.num_frames ):
            mapping = self.__frame_mapping[i]
            self.__revere_mapping[mapping] = i

    # Given a count of frames sent, return the corresponding frame number.
    def frame_mapping( self, count ):
        return self.__frame_mapping[ count % self.num_frames ]

    # Given a frame number, return the corresponding count, modulo patten length.
    def reverse_mapping( self, FN ):
        return self.__revere_mapping[ FN % self.repeat_length ]

    # Return true if this channel is allowed on this slot.
    def is_allowed_slot( self, slot ):
        return ( self.allowed_slots & ( 1 << slot ) )
