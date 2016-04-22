#ifndef __TDMA_H__
#define __TDMA_H__

#include "GSMCommon.h"

namespace GSM {

/**
	A description of a channel's multiplexing pattern.
	From GSM 05.02 Clause 7.
	This object encodes a line from tables 1-4 in the spec.
	The columns of interest in this encoding are:
		- 1, Channel Designation
		- 2, Subchannel
		- 3, Direction
		- 4, Allowable Time Slot Assignments
		- 5, Allowable RF Channel Assignments
		- 7, Repeat Length in TDMA Frames
		- 8, Interleaved Block TDMA Frame Mapping

	Col 6, Burst Type, is implied by 1 & 2 and encoded into the transcevier source code.
*/
class TDMAMapping {

	public:

	/// The longest "repeat length" of any channel we support is 104 for the SACCH/TF.
	static const unsigned mMaxRepeatLength = 104;

	private:

	TypeAndOffset mTypeAndOffset;				///< col 1, 2, encoded as per GSM 04.08 10.5.2.5
	bool mDownlink;								///< col 3, true for downlink channels
	bool mUplink;								///< col 3, true for uplink channels
	char mAllowedSlots;							///< col 4, an 8-bit mask
	bool mC0Only;								///< col 5, true if channel is limited to C0
	unsigned mRepeatLength;						///< col 7
	unsigned mNumFrames;						///< number of occupied frames in col 8
	const unsigned *mFrameMapping;				///< col 8
	unsigned mReverseMapping[mMaxRepeatLength];	///< index reversal of mapping, -1 means unused


	public:


	/**
		Construct a TDMAMapping, encoding one line of GSM 05.02 Clause 7 Tables 1-4.
		@param wTypeAndOffset Encoding of "Channel designnation".  See GSM 04.08 10.5.2.5.
		@param wDownlink True for downlink and bidirectional hannels
		@param wUplink True for uplink and bidirectional channels
		@param wRepeatLength "Repeat Length in TDMA Frames"
		@param wNumFrames Number of occupied TDMA frames in frame mapping.
		@param wFrameMapping "Interleaved Block TDMA Frame Mapping" -- MUST PERSIST!!
	*/
	TDMAMapping(TypeAndOffset wTypeAndOffset,
		bool wDownlink, bool wUplink, char wAllowedSlots, bool wC0Only,
		unsigned wRepeatLength, unsigned wNumFrames, const unsigned *wFrameMapping);

	/** Given a count of frames sent, return the corresponding frame number. */
	unsigned frameMapping(unsigned count) const
		{ return mFrameMapping[count % mNumFrames]; }

	/** Given a frame number, return the corresponding count, modulo patten length. */
	int reverseMapping(unsigned FN) const
		{ return mReverseMapping[FN % mRepeatLength]; }

	/**@name Simple accessors. */
	//@{
	unsigned numFrames() const { return mNumFrames; }

	unsigned repeatLength() const { return mRepeatLength; }

	TypeAndOffset typeAndOffset() const { return mTypeAndOffset; }

	bool uplink() const { return mUplink; }

	bool downlink() const { return mDownlink; }

	bool C0Only() const { return mC0Only; }
	//@}

	///< Return true if this channel is allowed on this slot.
	bool allowedSlot(unsigned slot) const
		{ return mAllowedSlots & (1<<slot); }
};

}

extern const GSM::TDMAMapping gRACHC5Mapping;

#endif
