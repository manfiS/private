#include "tdma.h"
#include "GSMCommon.h"

namespace GSM {

TDMAMapping::TDMAMapping(TypeAndOffset
		wTypeAndOffset, bool wDownlink, bool wUplink, char wAllowedSlots, bool wC0Only,
		unsigned wRepeatLength, unsigned wNumFrames, const unsigned *wFrameMapping)
	:mTypeAndOffset(wTypeAndOffset),
	mDownlink(wDownlink),mUplink(wUplink),mAllowedSlots(wAllowedSlots),mC0Only(wC0Only),
	mRepeatLength(wRepeatLength),mNumFrames(wNumFrames),mFrameMapping(wFrameMapping)
{
	// Sanity check.
	assert(mRepeatLength<=mMaxRepeatLength);

	// Default, -1, means a non-occupied position.
	for (unsigned i=0; i<mMaxRepeatLength; i++) mReverseMapping[i]=-1;

	// Fill in the reverse map, precomputed for speed.
	for (unsigned i=0; i<mNumFrames; i++) {
		unsigned mapping = mFrameMapping[i];
		assert(mapping<mRepeatLength);
		mReverseMapping[mapping] = i;
	}
}

}

// (pat) This comes from GSM 5.02 clause 7 table 5 of 9 page 46.
const unsigned RACHC5Frames[] = {4,5,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,45,46};

const GSM::TDMAMapping gRACHC5Mapping(
	GSM::TDMA_BEACON,
	false,
	true,
	0x1,
	true,
	51,
	sizeof(RACHC5Frames) / sizeof(unsigned),
	RACHC5Frames
);
