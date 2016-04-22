from um.gsm.bitvector import BitVector

class SoftVector:
    def __init__(self, data, normalize = True):
        self.size  = len(data)
        self.range = range(self.size)
        self.data  = [0.0] * self.size

        # soft-decision signal, values 0..1 represent probabilities
        # that a bit is "true".
        for i in self.range:
            if normalize is True:
                self.data[i] = ord(data[i]) / 256.0
            else:
                self.data[i] = data[i]

    def segment(self, index, size ):
        return SoftVector( self.data[index:index+size], False )

    def bits(self):
        bits = [0] * self.size
        for i in self.range:
            if self.data[i] > 0.5:
                bits[i] = 1
            else:
                bits[i] = 0
        return bits

    # Decode soft symbols with the GSM rate-1/2 Viterbi decoder.
    def decode(self, viterbi, osize):
        target = [0] * osize
        deferral = viterbi.DEFERRAL
        ctsz = self.size + deferral * viterbi.IRATE

        # Build a "history" array where each element contains the full history.
        history = [0] * ctsz
        bits = self.bits()
        accum = 0
        for i in self.range:
            accum = (accum << 1) | bits[i]
            history[i] = accum
        # Repeat last bit at the end.
        for i in range(self.size, ctsz):
            accum = (accum << 1) | (accum & 1)
            history[i] = accum

        # Precompute metric tables ( 0.5 = unknown )
        matchCostTable = [0.5] * ctsz
        mismatchCostTable = [0.5] * ctsz
        for i in self.range:
            # pval is the probability that a bit is correct.
            # ipval is the probability that a bit is incorrect.
            pval = self.data[i]
            if pval > 0.5:
                pval = 1.0 - pval
            ipval = 1.0 - pval
            # This is a cheap approximation to an ideal cost function.
            if pval < 0.01:
                pval = 0.01
            if ipval < 0.01:
                ipval = 0.01

            matchCostTable[i] = 0.25 / ipval
            mismatchCostTable[i] = 0.25 / pval

        # Each sample of history[] carries its history.
        # So we only have to process every iRate-th sample.
        viterbi.initialize_states()
        step = viterbi.IRATE
        mi = 0
        mmi = 0
        ii = step - 1
        oi = 0
        ocount = 0
        while oi < osize:
            min_cost = viterbi.step(history[ii], matchCostTable[mi:], mismatchCostTable[mmi:])
            ii += step
            mi += step
            mmi += step
            # output
            if ocount >= deferral:
                target[oi] = (min_cost.istate >> deferral) & 1
                oi += 1
            ocount += 1

        return BitVector( target )