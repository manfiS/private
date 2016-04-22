#ifndef _RACH_H_
#define _RACH_H_

#include "decoder.h"

namespace GSM {
namespace L1 {

class RACHBurst {
public:
  unsigned      bsic;
  GSM::Time     time;
  int           rssi;
  int           toa;
  unsigned char ra;

  RACHBurst( unsigned bsic, GSM::Time time, int rssi, int toa, unsigned char ra );

  std::string to_s();
};

/** L1 decoder for Random Access (RACH). */
class RACHDecoder : public L1::Decoder {

	private:


	ViterbiR2O4 mVCoder;	///< nearly all GSM channels use the same convolutional code
	Parity mParity;					///< block coder
	BitVector mU;					///< u[], as per GSM 05.03 2.2
	BitVector mD;					///< d[], as per GSM 05.03 2.2

	ChannelType channelType() const { return RACHType; }

	public:

	RACHDecoder(const TDMAMapping &wMapping, unsigned wTN) : L1::Decoder(0,wTN,wMapping),
		mParity(0x06f,6,8),
    mU(18),
    mD(mU.head(8))
	{ }

	void writeLowSideRx(const RxBurst&);
};

}
}

#endif
