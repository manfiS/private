#ifndef _L1_DECODER_H_
#define _L1_DECODER_H_

#include "tdma.h"
#include "GSMTransfer.h"

namespace GSM {
namespace L1 {

struct HandoverRecord {
	bool mValid;
	float mhrRSSI;
	float mhrTimingError;
	double mhrTimestamp;
	HandoverRecord() : mValid(false) {}
	HandoverRecord(float rssi,float te, double timestamp) : mValid(true), mhrRSSI(rssi), mhrTimingError(te), mhrTimestamp(timestamp) {}
};

enum EncryptionType {
	ENCRYPT_NO,
	ENCRYPT_MAYBE,
	ENCRYPT_YES
};

struct DecoderStats {
	float mAveFER;
	float mAveBER;
	float mAveSNR;
	float mLastSNR;	// Most recent SNR
	float mLastBER;	// Most recent BER
	unsigned mSNRCount;	// Number of SNR samples taken.
	int mStatTotalFrames;
	int mStatStolenFrames;
	int mStatBadFrames;
	void decoderStatsInit();
};

// Base class for L1 decoders.
class Decoder {
protected:

	/**@name Mutex-controlled state information. */
	//@{
	mutable Mutex mDecLock;				///< access control
	/**@name Timers from GSM 04.08 11.1.2 */
	// (pat) GSM 4.08 3.4.13.1.1 is confusing, lets define some terms:
	// "main signalling disconnected" means at layer 2, which can be either by the normal release procedure
	// that sends a LAPDm DISC and waits for a response, or by "local end release" which means drop the channel immediately.
	// T3109 is either: 3.4.13.1.1 the time between deactivation of the SACCH and when the channel is considered recyclable, in which
	//   case we are relying on the MS to release the channel after RADIO-LINK-TIMEOUT missing SACCH messages,
	// or 3.4.13.2.2: the time after detecting a radio failure (by SACCH loss or bad RXLEV as per 5.08 5.2) and when
	// the channel is considered recylcable.
	// T3111 is shorter than T3109, used when a normal RELEASE is successfully acknoledged by the handset,
	// so we dont have to wait the entire T3109 time.
	//
	//@{
	//Z100Timer mT3101;					///< timer for new channels
	//Z100Timer mT3109;					///< timer for loss of uplink signal.  Need to check both host and SACCH timers.
	//Z100Timer mT3111;					///< timer for reuse of a normally closed channel
	Z100Timer mT3103;					///< timer for handover
	//@}
	bool mDecActive;					///< true between open() and close()
	//@}

	/**@name Atomic volatiles, no mutex. */
	// Yes, I realize we're violating our own rules here. -- DAB
	//@{
	//volatile float mFER;						///< current FER estimate
	//static const int mFERMemory=208;			///< FER decay time, in frames
				// (pat) Uh, no.  It is in frames for control frames, but in frames/4 for TCH frames.
				// (pat) Upped it from 20 frames to 208 frames, ie, about a second.
	static const int mSNRMemory = 208;
	volatile bool mHandoverPending;				///< if true, we are decoding handover bursts
	volatile unsigned mHandoverRef;
	HandoverRecord mHandoverRecord;
	//@}

	/**@name Parameters fixed by the constructor, not requiring mutex protection. */
	//@{
	unsigned mCN;					///< carrier index
	unsigned mTN;					///< timeslot number
	const TDMAMapping& mMapping;	///< demux parameters
	L1FEC* mParent;			///< a containing L1 processor, if any
	/** The channel type. */
	virtual ChannelType channelType() const = 0;
	//@}

	// (pat) Moved to classes that use the convolutional coder.
	//ViterbiR2O4 mVCoder;	///< nearly all GSM channels use the same convolutional code

	EncryptionType mEncrypted;
	int mEncryptionAlgorithm;	// Algorithm number, ie 1 means A5_1, 2 means A5_2, etc.
	unsigned char mKc[8];
	int mFN[8];

	DecoderStats mDecoderStats;

	public:

	/**
		Constructor for an Decoder.
		@param wTN The timeslot to decode on.
		@param wMapping Demux parameters, MUST BE PERSISTENT.
		@param wParent The containing L1FEC, for sibling access.
	*/
	Decoder(unsigned wCN, unsigned wTN, const TDMAMapping& wMapping)
			://mT3101(T3101ms),
			//mT3109(gConfig.GSM.Timer.T3109),
			//mT3111(T3111ms),
      // Handover timeout in milliseconds, GSM 04.08 11.1.2.
      // This is the timeout for a handset to seize a channel during handover.
      mT3103(12000),
			mDecActive(false),
			//mRunning(false),
			//mFER(0.0F),
			mCN(wCN),mTN(wTN),
			mMapping(wMapping),
			mEncrypted(ENCRYPT_NO),
			mEncryptionAlgorithm(0)
	{
		// Start T3101 so that the channel will
		// become recyclable soon.
		//mT3101.set();
	}


	virtual ~Decoder() { }


	/**
		Clear the decoder for a new transaction.
		Start T3101, stop the others.
	*/
	virtual void decInit();
	void decStart();

	/**
		Call this at the end of a tranaction.
		Stop timers.  If releaseType is not hardRelase, start T3111.
	*/
	virtual void close();	// (pat) why virtual?

	public:
	/**
		Returns true if the channel is in use for a transaction.
		Returns true if T3111 is not active.
	*/
	bool decActive() const;

	/** Total frame error rate since last open(). */
	float FER() const { return mDecoderStats.mAveFER; }			// (pat) old routine, switched to new variable.
	DecoderStats getDecoderStats() const { return mDecoderStats; }	// (pat) new routine, more stats

	/** Return the multiplexing parameters. */
	const TDMAMapping& mapping() const { return mMapping; }

	/** Accept an RxBurst and process it into the deinterleaver. */
	virtual void writeLowSideRx(const RxBurst&) = 0;

	/**@name Components of the channel description. */
	//@{
	unsigned TN() const { return mTN; }
	unsigned ARFCN() const;                 ///< this comes from mUpstream
	TypeAndOffset typeAndOffset() const;    ///< this comes from mMapping
	//@}

	/** Control the processing of handover access bursts. */
	// flag is true if handover is pending, false otherwise.
	// OK to pass reference to HandoverRecord since this struct is immortal.
	HandoverRecord &handoverPending(bool flag, unsigned handoverRef)
	{
		if (flag) { mT3103.set(12000); } else { mT3103.reset(); }
		mHandoverPending=flag;
		mHandoverRef=handoverRef;
		return mHandoverRecord;
	}

	public:
	const L1FEC* parent() const { return mParent; }
	L1FEC* parent() { return mParent; }

	/** How much time left in T3101? */
	//long debug3101remaining() { return mT3101.remaining(); }

	protected:

	public:
	// (pat 5-2014) Count bad frames in the BTS the same way as documented for RADIO-LINK-TIMEOUT in the MS, namely,
	// increment by one for each bad frame, decrement by two for each good frame.  This allows detecting marginal
	// conditions, whereas the old way of just using a timer that was reset only required one good frame every timer period,
	// which really only detected total channel loss, not a marginal channel.  This var is only used on SACCH,
	// so we dont bother modifying it in countStolenFrame, since stolen frames are only on TCH/FACCH.
	// Note: Spec says frames are always sent on SACCH in both directions as long as channel is up,
	// which is not necessarily required on the associated SDCCH or TCH/FACCH.
	// Note: Unlike the counts in DecoderStats we are counting message periods, which are 4-frame groups, not individual frames.
	int mBadFrameTracker;
	void countGoodFrame(unsigned nframes=1);
	virtual void countBadFrame(unsigned nframes = 1);	// over-ridden by SACCHDecoder
	void countStolenFrame(unsigned nframes = 1);
	void countBER(unsigned bec, unsigned frameSize);

	bool decrypt_maybe(std::string wIMSI, int wA5Alg);
	unsigned char *kc() { return mKc; }
};

}
}

#endif
