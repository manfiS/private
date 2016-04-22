#include "decoder.h"

namespace GSM {
namespace L1 {

void DecoderStats::decoderStatsInit() {
	mAveFER = 0;
	mAveSNR = 0;
	mAveBER = 0;
	mSNRCount = 0;
	mStatTotalFrames = 0;
	mStatStolenFrames = 0;
	mStatBadFrames = 0;
}

// Given IMSI, copy Kc.  Return true iff there *is* a Kc.
bool imsi2kc(std::string wIMSI, unsigned char *wKc)
{
	/*
	std::string kc = gTMSITable.getKc(wIMSI.c_str());
	if (kc.length() == 0) return false;
	while (kc.length() < 16) {
		kc = '0' + kc;
	}
	assert(kc.length() == 16);
	unsigned char *dst = wKc;
	for (size_t p = 0; p < kc.length(); p += 2) {
		*dst++ = (unhex(kc[p]) << 4) | (unhex(kc[p+1]));
	}

	return true;
	*/
	return false;
}

// Turn on encryption phase-in, which is watching for bad frames and
// retrying them with encryption.
// Return false and leave encryption off if there's no Kc.
bool Decoder::decrypt_maybe(std::string wIMSI, int wA5Alg)
{
	if (!imsi2kc(wIMSI, mKc)) return false;
	mEncrypted = ENCRYPT_MAYBE;
	mEncryptionAlgorithm = wA5Alg;
	return true;
}


unsigned Decoder::ARFCN() const
{
	return 0;
}


TypeAndOffset Decoder::typeAndOffset() const
{
	return mMapping.typeAndOffset();
}

void Decoder::decInit()
{
	handoverPending(false,0);
	ScopedLock lock(mDecLock);
	//if (!mRunning) decStart();
	//mRunning = true;
	mDecoderStats.decoderStatsInit();
	//mFER=0.0F;
	mBadFrameTracker = 0;
	//mT3111.reset();
	//mT3109.reset(gConfig.GSM.Timer.T3109);
	//mT3101.reset();
	// Turning off encryption when the channel closes would be a nightmare
	// (catching all the ways, and performing the handshake under less than
	// ideal conditions), so we leave encryption on to the bitter end,
	// then clear the encryption flag here, when the channel gets reused.
	mEncrypted = ENCRYPT_NO;
	mEncryptionAlgorithm = 0;
	//mActive = true;
}

void Decoder::decStart()
{
	//mT3111.reset();
	// Pat changed initial open state on T3109 from inactive via reset to active via set,
	// so that it is easier to test in GSMLogicalChannel.
	//mT3109.set(); 		//old: mT3109.reset();
	//mT3101.set();
	mDecActive = true;
}


void Decoder::close()
{
	mDecActive = false;
}

bool Decoder::decActive() const
{
	return mDecActive;
}

void Decoder::countStolenFrame(unsigned nframes)
{
	// (pat 1-16-2014) Stolen frames should not affect FER reporting.
	mDecoderStats.mStatTotalFrames += nframes;
	mDecoderStats.mStatStolenFrames += nframes;
}

// How many frames we average FER, minus 1.  For reporting.
static const unsigned cFERMemory = 208;

void Decoder::countGoodFrame(unsigned nframes)	// Number of bursts to count.
{
	// Subtract 2 for each good frame, but dont go below zero.
	mBadFrameTracker = mBadFrameTracker <= 1 ? 0 : mBadFrameTracker-2;
	static const float a = 1.0F / ((float)cFERMemory);
	static const float b = 1.0F - a;
	mDecoderStats.mAveFER *= b;
	mDecoderStats.mStatTotalFrames += nframes;
}

void Decoder::countBER(unsigned bec, unsigned frameSize)
{
	static const float a = 1.0F / ((float)cFERMemory);
	static const float b = 1.0F - a;
	float thisBER = (float) bec / frameSize;
	mDecoderStats.mLastBER = thisBER;
	mDecoderStats.mAveBER = b*mDecoderStats.mAveBER + a * thisBER;
}


void Decoder::countBadFrame(unsigned nframes)	// Number of bursts to count.
{
	mBadFrameTracker++;
	static const float a = 1.0F / ((float)cFERMemory);
	static const float b = 1.0F - a;
	mDecoderStats.mAveFER = b*mDecoderStats.mAveFER + a;
	mDecoderStats.mStatTotalFrames += nframes;
	mDecoderStats.mStatBadFrames += nframes;
}


}
}
