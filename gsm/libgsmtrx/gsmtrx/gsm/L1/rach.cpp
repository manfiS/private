#include "rach.h"
#include "gsmtrx.h"
#include <stdio.h>

namespace GSM {
namespace L1 {


/**
 * C++ version 0.4 char* style "itoa":
 * Written by Lukás Chmela
 * Released under GPLv3.
 */
char* itoa(int value, char* result, int base) {
	// check that the base if valid
	if (base < 2 || base > 36) { *result = '\0'; return result; }

	char* ptr = result, *ptr1 = result, tmp_char;
	int tmp_value;

	do {
		tmp_value = value;
		value /= base;
		*ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
	} while ( value );

	// Apply negative sign
	if (tmp_value < 0) *ptr++ = '-';
	*ptr-- = '\0';
	while(ptr1 < ptr) {
		tmp_char = *ptr;
		*ptr--= *ptr1;
		*ptr1++ = tmp_char;
	}
	return result;
}

RACHBurst::RACHBurst( unsigned bsic, GSM::Time time, int rssi, int toa, unsigned char ra ) :
	bsic(bsic),
	time(time),
	rssi(rssi),
	toa(toa),
	ra(ra){

}

std::string RACHBurst::to_s() {
	char s[0xFF] = {0};

  int n = sprintf( s, "[L1::RACH] TS=%d FN=%d RSSI=%d TOA=%d BSIC=%d RA=b", time.TN(), time.FN(), rssi, toa, bsic );
	if( n ){
		char *p = &s[n];
		for( int i = 0; i < 8; ++i, ++p ) {
      sprintf( p, "%d", !!((ra << i) & 0x80));
  	}
	}

	return std::string(s);
}

void RACHDecoder::writeLowSideRx(const RxBurst& burst)
{
	// The L1 FEC for the RACH is defined in GSM 05.03 4.6.

	// Decode the burst.
	const SoftVector e(burst.segment(49,36));
	e.decode(mVCoder,mU);

	// To check validity, we have 4 tail bits and 6 parity bits.
	// False alarm rate for random inputs is 1/1024.

	// Check the tail bits -- should all the zero.
	if (mU.peekField(14,4)) {
		countBadFrame(1);
		return;
	}

	// Check the parity.
	// The parity word is XOR'd with the BSIC. (GSM 05.03 4.6.)
	unsigned sentParity = ~mU.peekField(8,6);
	unsigned checkParity = mD.parity(mParity);
	unsigned encodedBSIC = (sentParity ^ checkParity) & 0x03f;

	// We got a valid RACH burst.
	// The "payload" is an 8-bit field, "RA", defined in GSM 04.08 9.1.8.
	// The channel assignment procedure is in GSM 04.08 3.3.1.1.3.
	// It requires knowledge of the RA value and the burst receive time.
	// The RACH L2 is so thin that we don't even need code for it.

	countGoodFrame(1);
	mD.LSB8MSB();
	unsigned RA = mD.peekField(0,8);

  GSMTRX::get().onL1RACHBurst( L1::RACHBurst( encodedBSIC, burst.time(), burst.RSSI(), burst.timingError(), RA ) );
}

}
}
