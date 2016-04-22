# Shift-register (LFSR) generator.
class Generator:
    def __init__(self, coeff, len):
        # polynomial coefficients. LSB is zero exponent.
        self.coeff = coeff
        # shift register state. LSB is most recent.
        self.__state = 0
        # mask for reading state
        self.mask = ( 1 << len ) - 1
        # number of bits used in shift register
        self.len = len
        # len - 1
        self.len_1 = len - 1

    def clear(self):
        self.__state = 0

    def state(self):
        return self.__state & self.mask

    def syndrome_shift(self, inbit):
        # Calculate one bit of a syndrome.
        fb = ( self.__state >> self.len_1 ) & 1
        self.__state = ( self.__state << 1 ) ^ ( inbit & 1 )
        if fb:
            self.__state ^= self.coeff

    def encoder_shift(self, inbit):
        # Update the generator state by one cycle.
        fb = (( self.__state >> self.len_1 ) ^ inbit ) & 1
        self.__state <<= 1
        if fb:
            self.__state ^= self.coeff

# Parity (CRC-type) generator and checker based on a Generator.
class Parity(Generator):
    def __init__(self, coefficients, parity_size, codeword_size):
        Generator.__init__( self, coefficients, parity_size )
        self.codeword_size = codeword_size

class BitVector:
    def __init__(self, bits):
        self.bits = bits
        self.size = len(bits)
        self.range = range(self.size)

    def segment(self, index, size):
        return BitVector(self.bits[index:index + size])

    def peek_field(self, index, length):
        accum = 0
        for i in range(length):
            accum = ( accum << 1 ) | ( self.bits[index] & 1 )
            index += 1
        return accum

    def parity(self, generator):
        generator.clear()
        for i in self.range:
            generator.encoder_shift( self.bits[i] )
        return generator.state()

    def lsb8msb(self):
        # Reverse groups of 8 within the vector (byte reversal).
        if self.size >= 8:
            size8 = 8 * ( self.size / 8 )
            itop  = size8 - 8
            for i in range(0,itop + 1,8):
                self.bits[i],     self.bits[i + 7] = self.bits[i + 7], self.bits[i]
                self.bits[i + 1], self.bits[i + 6] = self.bits[i + 6], self.bits[i + 1]
                self.bits[i + 2], self.bits[i + 5] = self.bits[i + 5], self.bits[i + 2]
                self.bits[i + 3], self.bits[i + 4] = self.bits[i + 4], self.bits[i + 3]




