# A candidate sequence in a Viterbi decoder.
# The 32-bit state register can support a deferral of 6 with a 4th-order coder.
class Candidate:
    def __init__(self):
        # encoder input associated with this candidate
        self.istate = 0
        # encoder output associated with this candidate
        self.ostate = 0
        # cost (metric value), float to support soft inputs
        self.cost   = 0

# Class to represent convolutional Viterbi coders/decoders of rate 1/2, memory length 4.
class Viterbi:
    # reciprocal of rate
    IRATE = 2
    # memory length of generators
    ORDER = 4
    # number of states/survivor
    ISTATES = 1 << ORDER
    # survivor mask
    SMASK = ISTATES - 1
    # candidate mask
    CMASK = ( SMASK << 1 ) | 1
    # output mask, all irate low bits set
    OMASK = ( 1 << IRATE ) - 1
    # number of candidates to generate during branching
    NUMCANDS = ISTATES * 2
    # deferral to be used
    DEFERRAL = 6 * ORDER

    # TODO: Precompute ranges.

    def __init__(self):
        # polynomial for each generator
        self.coeffs = ( 0x019, 0x01b )
        # precomputed generator output tables
        self.state_table = [[0] * self.NUMCANDS] * self.IRATE
        # precomputed coder output table
        self.generator_table = [0] * self.NUMCANDS
        # current survivor pool
        self.survivors = [Candidate()] * self.ISTATES
        # current candidate pool
        self.candidates = [Candidate()] * self.NUMCANDS

        self.__compute_state_tables(0)
        self.__compute_state_tables(1)
        self.__compute_generator_table()

    def initialize_states(self):
        self.survivors  = [Candidate()] * self.ISTATES
        self.candidates = [Candidate()] * self.NUMCANDS

    def step(self, isample, probs, iprobs ):
        self.__branch_candidates()
        self.__get_softcost_metrics( isample, probs, iprobs )
        self.__prune_candidates()
        return self.__min_cost()

    def __branch_candidates(self):
        # Branch to generate new input states.
        sp = 0
        for i in range( 0, self.NUMCANDS, 2 ):
            # extend and suffix
            # input for state 0
            istate0 = self.survivors[sp].istate  << 1
            # input for state 1
            istate1 = istate0 | 1
            # shifted output
            ostate_shifted = self.survivors[sp].ostate << self.IRATE

            cost = self.survivors[sp].cost
            sp += 1
            # 0 input extension
            self.candidates[i].cost   = cost
            self.candidates[i].ostate = ostate_shifted | self.generator_table[istate0 & self.CMASK]
            self.candidates[i].istate = istate0
            # 1 input extension
            self.candidates[i+1].cost = cost
            self.candidates[i+1].ostate = ostate_shifted | self.generator_table[istate1 & self.CMASK]
            self.candidates[i+1].istate = istate1

    def __get_softcost_metrics(self, isample, match_cost, mismatch_cost ):
        ctab = ( match_cost, mismatch_cost )
        for i in range(self.NUMCANDS):
            # We examine input bits 2 at a time for a rate 1/2 coder
            mismatched = isample ^ self.candidates[i].ostate
            self.candidates[i].cost = ctab[mismatched&1][1] + ctab[(mismatched>>1)&1][0]

    def __prune_candidates(self):
        # 0-prefix
        c1 = 0
        # 1-prefix
        c2 = self.ISTATES
        for i in range( self.ISTATES ):
            a = self.candidates[c1 + i]
            b = self.candidates[c2 + i]
            if a.cost < b.cost:
                self.survivors[i] = a
            else:
                self.survivors[i] = b

    def __min_cost(self):
        min_index = 0
        min_cost = self.survivors[0].cost
        for i in range(1, self.ISTATES):
            c = self.survivors[i].cost
            if c < min_cost:
                min_cost = c
                min_index = i
        return self.survivors[min_index]

    def __apply_poly(self, val, poly, order):
        # Apply a Galois polymonial to a binary seqeunce.
        prod = val & poly
        acc = prod
        for i in range(1,order):
            acc ^= prod >> i
        return acc & 1

    def __compute_state_tables(self, g):
        for state in range(self.ISTATES):
            # 0 input
            ival = state << 1
            self.state_table[g][ival] = self.__apply_poly( ival, self.coeffs[g], self.ORDER + 1 )
            # 1 input
            ival |= 1
            self.state_table[g][ival] = self.__apply_poly( ival, self.coeffs[g], self.ORDER + 1 )

    def __compute_generator_table(self):
        for i in range(self.NUMCANDS):
            self.generator_table[i] = ( self.state_table[0][i] << 1 ) | self.state_table[1][i]




