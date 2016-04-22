/*
* Copyright 2008, 2009 Free Software Foundation, Inc.
* Copyright 2011, 2012 Range Networks, Inc.
*
* This software is distributed under multiple licenses; see the COPYING file in the main directory for licensing information for this specific distribuion.
*
* This use of this software may be subject to additional restrictions.
* See the LEGAL file in the main directory for details.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/
#include <stdio.h>
#include "Transceiver.h"
#include <gsmtrx.h>

Transceiver::Transceiver(
			 int wSamplesPerSymbol,
			 GSM::Time wTransmitLatency,
			 RadioInterface *wRadioInterface,
			 unsigned int wNumARFCNs,
			 unsigned int wOversamplingRate,
			 bool wLoadTest)
{
  //GSM::Time startTime(0,0);
  //GSM::Time startTime(gHyperframe/2 - 4*216*60,0);
  GSM::Time startTime = mStartTime = GSM::Time(random() % gHyperframe,0);

  mMultipleARFCN = (wNumARFCNs > 1 || wOversamplingRate > 1);
  mFIFOServiceLoopThread = new Thread(2*32768);  ///< thread to push bursts into transmit FIFO
  mRFIFOServiceLoopThread = new Thread(4*32768);
  for (unsigned int j = 0; j< wNumARFCNs; j++) {
    if (mMultipleARFCN) mDemodServiceLoopThread[j] = new Thread(32768);
    mDemodFIFO[j] = new VectorFIFO;
  }

  mSamplesPerSymbol = wSamplesPerSymbol;
  mRadioInterface = wRadioInterface;
  mTransmitLatency = wTransmitLatency;
  mTransmitDeadlineClock = startTime;
  mLastClockUpdateTime = startTime;
  mLatencyUpdateTime = startTime;
  mRadioInterface->getClock()->set(startTime);
  mMaxExpectedDelay = 1;

  mNumARFCNs = wNumARFCNs;
  mOversamplingRate = wOversamplingRate;
  mLoadTest = wLoadTest;

  LOG(NOTICE, "Running %d ARFCNs with oversampling %d", mNumARFCNs, wOversamplingRate );

  // generate pulse and setup up signal processing library
  gsmPulse = generateGSMPulse(2,mSamplesPerSymbol);

  sigProcLibSetup(mSamplesPerSymbol);

  txFullScale = mRadioInterface->fullScaleInputValue();
  rxFullScale = mRadioInterface->fullScaleOutputValue();

  mFreqOffset = 0.0;

  // initialize other per-timeslot variables
  for (int tn = 0; tn < 8; tn++) {
	  for (unsigned int arfcn = 0; arfcn < mNumARFCNs; arfcn++) {
		mFillerModulus[arfcn][tn] = 26;
		mChanType[arfcn][tn] = NONE;
		//fillerActive[arfcn][tn] = (arfcn==0);
		mHandoverActive[arfcn][tn] = false;
	  }
  }

  if (mMultipleARFCN) {
		// Create the "tones" for sub-band tuning multiple ARFCNs.
		//mOversamplingRate = mNumARFCNs/2 + mNumARFCNs;
		//mOversamplingRate = 15; //mOversamplingRate*4;
		//if (mOversamplingRate % 2) mOversamplingRate++;
		double beaconFreq = -1.0*(mNumARFCNs-1)*200e3;
		for (unsigned int j = 0; j < mNumARFCNs; j++) {
		  mFrequencyShifter[j] = new signalVector(157*mOversamplingRate);
		  mFrequencyShifter[j]->fill(complex(1.0,0.0));
		  mFrequencyShifter[j]->isRealOnly(false);
		  frequencyShift(mFrequencyShifter[j],mFrequencyShifter[j],2.0*M_PI*(beaconFreq+j*400e3)/(1625.0e3/6.0*mOversamplingRate));
		}

		int filtLen = 6*mOversamplingRate;
		decimationFilter = createLPF(0.5/mOversamplingRate,filtLen,1);
		interpolationFilter = createLPF(0.5/mOversamplingRate,filtLen,1);
		scaleVector(*interpolationFilter,mOversamplingRate);
		mFreqOffset = -beaconFreq;
		mRadioInterface->setSamplesPerSymbol(SAMPSPERSYM*mOversamplingRate);
  } // if (mMultipleARFCN)

	// initialize filler tables with dummy bursts.

	for (unsigned int cn = 0; cn < mNumARFCNs; cn++) {
	  for (int tn = 0; tn < 8; tn++) {
		signalVector* modBurst = modulateBurst(gDummyBurst,*gsmPulse,
										   8 + (tn % 4 == 0),
										   mSamplesPerSymbol);
		// Power-scale, resample and frequency-shift.
		// Note that these are zero-power bursts on cn other than c0.
		// FIXME -- It would be cleaner to handle cn>0 in a different loop.
		// Otherwise, we as wasting a lot of computation here.
		if (cn==0) scaleVector(*modBurst,txFullScale/mNumARFCNs);
		else scaleVector(*modBurst,0.0);
		if (mMultipleARFCN) {
		  signalVector *interpVec = polyphaseResampleVector(*modBurst,mOversamplingRate,1,interpolationFilter);
		  //signalVector *interpVec = new signalVector(modBurst->size()*mOversamplingRate);
		  //interpVec->fill(txFullScale);
		  multVector(*interpVec,*mFrequencyShifter[cn]);
		  delete modBurst;
		  modBurst = interpVec;
		}
		for (int fn = 0; fn < MAXMODULUS; fn++) {
		  mFillerTable[cn][fn][tn] = new signalVector(*modBurst);
		}
		delete modBurst;
	  }
	}

  mOn = false;
  mTxFreq = 0.0;
  mRxFreq = 0.0;
  mPower = -10;

  mControlLock.unlock();
  mTransmitPriorityQueueLock.unlock();
}

Transceiver::~Transceiver()
{
  delete gsmPulse;
  sigProcLibDestroy();
  mTransmitPriorityQueue.clear();
}


radioVector *Transceiver::fixRadioVector(BitVector &burst,
				 int RSSI,
				 GSM::Time &wTime,
				 int ARFCN)
{

	// DAVID COMMENT: The way that modulateBurst is used in the multi-ARFCN
	// transceiver, mSamplesPerSymbol is effectively hard-coded to 1.
	//
	// DAVID COMMENT: If we ran modulateBurst at the oversampled rate, instead
	// of at one sample per symbol and then upsampling later, the signal would
	// be cleaner and the code would be more efficient.  In other words, we could
	// oversample the binary data stream (cheap and exact) instead of oversampling
	// the waveforms later (complex and potentially noisy).

  // modulate and stick into queue
  signalVector* modBurst = modulateBurst(burst,*gsmPulse,
					 8 + (wTime.TN() % 4 == 0),
					 mSamplesPerSymbol);
  /*complex rScale = complex(2*M_PI*((float) rand()/(float) RAND_MAX),(2*M_PI*((float) rand()/(float) RAND_MAX)));
  rScale = rScale/rScale.abs();
  scaleVector(*modBurst,rScale);*/

  // DAVID COMMENT: Note that amplitude is scaled by number of ARFCNs, with an additional
  // factor of 2 down-scaling for >1 ARFCN.
  //
  // DAVID COMMENT: Where did txFullScale come from?  You can't tell from the name, but it's actually
  // an instance variable of Transceiver.  It was set from mRadioInterface->fullScaleOutputValue()
  // back in the constructor, which comes from the radio device class.  BladeRF "correctly" sets this
  // to 2040.  Why is "correct" in quotes?  See next comment.
  //
  // DAVID COMMENT: Note now that the radioVector is scaled +/- 2040 and all of the gains and
  // frequency shift vectors applied to it are scaled +/- 1.  Then, just before transmission, the
  // samples, supposedly in the +/- 2040 range, are rounded to the nearest integer.  A much cleaner
  // and less error-prone approach would be to do all of the signal processing math in a +/- 1 scale
  // and then let the read/write methods on the device itself scale to whatever it expects.
  //
  float headRoom = mMultipleARFCN ? 0.5 : 1.0;
  scaleVector(*modBurst,txFullScale * headRoom * pow(10,-RSSI/10)/mNumARFCNs);
  radioVector *newVec = new radioVector(*modBurst,wTime,ARFCN);
  //fillerActive[ARFCN][wTime.TN()] = (ARFCN==0) || (RSSI != 255);

  // DAVID COMMENT: This is the place where the modulated signal is resampled and frequency-shifted
  // to give multi-ARFCN support in software.
  //
  // DAVID COMMENT: Note that we are applying a polyphase filter but with one side of the ratio
  // hard-coded to 1.  A dedicated upsampler or downsampler would be more efficient, but this
  // allowed a single resampler to be used for conversion in either direction, upsample on tx
  // and downsample on rx.
  //
  // upsample and filter and freq shift
  if (mMultipleARFCN) {
    signalVector *interpVec = polyphaseResampleVector(*((signalVector *)newVec),mOversamplingRate,1,interpolationFilter);
    delete newVec;

    //if (ARFCN!=0) printf("ARFCN: %d\n",ARFCN);
    multVector(*interpVec,*mFrequencyShifter[ARFCN]);

    newVec = new radioVector(*interpVec,wTime,ARFCN);
    delete interpVec;
  }
  delete modBurst;
  return newVec;
}

// If force, set the mFillerTable regardless of channel.
// If allocate, must allocate a copy of the incoming vector.
void Transceiver::setFiller(radioVector *rv, bool allocate, bool force)
{
	int CN = rv->ARFCN();
	int TN = rv->time().TN() & 0x07;	// (pat) Changed to 0x7 from 0x3.
	if (!force && (IGPRS == mChanType[CN][TN])) {
		LOG(INFO, "setFiller ignored: CN=%d TN=%d", CN, TN );
		if (!allocate) { delete rv; }
		return;
	}

	int modFN = rv->time().FN() % mFillerModulus[CN][TN];
	delete mFillerTable[CN][modFN][TN];
	if (allocate) {
		mFillerTable[CN][modFN][TN] = new signalVector(*rv);
	} else {
		mFillerTable[CN][modFN][TN] = rv;
	}
}

void Transceiver::pushRadioVector(GSM::Time &nowTime)
{
  // Transmit any enqueued bursts in this timeslot, on all ARFCNs.

  // dump stale bursts, if any
  while (radioVector* staleBurst = mTransmitPriorityQueue.getStaleBurst(nowTime)) {
    // Even if the burst is stale, put it in the filler table.
    // (It might be an idle pattern.)
    setFiller(staleBurst,false,false);
  }


  // Everything from this point down operates in one TN period,
  // across multiple ARFCNs in freq.
  int TN = nowTime.TN();

  // Sum up ARFCNs that are ready to transmit in this FN and TN
  bool addFiller[mNumARFCNs];
  for (unsigned int CN=0; CN<mNumARFCNs; CN++) addFiller[CN]=true;
  radioVector *sendVec = NULL;
  // if queue contains data at the desired timestamp, stick it into FIFO
  while (radioVector *next = (radioVector*) mTransmitPriorityQueue.getCurrentBurst(nowTime)) {
	  //
	  // DAVID COMMENT: This is the part where the mutli-ARFCN signal gets summed up for
	  // transmission.  Note that they are already resampled and frequency-shifted.
	  //
    unsigned int CN = next->ARFCN();
	if (CN >= mNumARFCNs) {
	  LOG(ERR, "Attempt to send burst on illegal ARFCN. C%d T%d FN %d", CN, TN, nowTime.FN() );
      delete next;
	  continue;
	}
	if (addFiller[CN] == false) {
	  LOG(ERR, "Attempt to send multiple bursts on C%d T%d FN %d", CN, TN, nowTime.FN() );
      delete next;
	  continue;
	}

    setFiller(next,true,false);
    addFiller[CN] = false;
    if (!sendVec) {
      sendVec = next;
    } else {
      addVector(*sendVec,*next);
      delete next;
    }
  }

  // generate filler on ARFCNs where we didn't get anything.
  for (unsigned int CN=0; CN<mNumARFCNs; CN++) {

    // Don't need filler?
    if (!addFiller[CN]) continue;

    // pull filler data, and set it up to be transmitted
    unsigned int modFN = nowTime.FN() % mFillerModulus[CN][TN];
    radioVector *tmpVec = new radioVector(*mFillerTable[CN][modFN][TN],nowTime,(int&)CN);
    if (!sendVec) {
      sendVec = tmpVec;
    } else {
       addVector(*sendVec,*tmpVec);
       delete tmpVec;
    }
  }

  // What if sendVec is still NULL?
  // It can't be if there are no NULLs in the filler table.
  mRadioInterface->driveTransmitRadio(*sendVec,false);
  delete sendVec;

}

void Transceiver::setModulus(int arfcn, int timeslot)
{

  switch (mChanType[arfcn][timeslot]) {
  case NONE:
  case I:
  case II:
  case III:
  case FILL:
  case IGPRS:
    mFillerModulus[arfcn][timeslot] = 26;
    break;
  case IV:
  case VI:
  case V:
    mFillerModulus[arfcn][timeslot] = 51;
    break;
    //case V:
  case VII:
    mFillerModulus[arfcn][timeslot] = 102;
    break;
  default:
    break;
  }
}


CorrType Transceiver::expectedCorrType(GSM::Time currTime, int arfcn)
{

  unsigned burstTN = currTime.TN();
  unsigned burstFN = currTime.FN();

  if (mHandoverActive[arfcn][burstTN]) return RACH;

  switch (mChanType[arfcn][burstTN]) {
  case NONE:
    return OFF;
    break;
  case FILL:
    return IDLE;
    break;
  case IGPRS:
  case I:
    return TSC;
    /*if (burstFN % 26 == 25)
      return IDLE;
    else
      return TSC;*/
    break;
  case II:
    if (burstFN % 2 == 1)
      return IDLE;
    else
      return TSC;
    break;
  case III:
    return TSC;
    break;
  case IV:
  case VI:
    return RACH;
    break;
  case V: {
    int mod51 = burstFN % 51;
    if ((mod51 <= 36) && (mod51 >= 14))
      return RACH;
    else if ((mod51 == 4) || (mod51 == 5))
      return RACH;
    else if ((mod51 == 45) || (mod51 == 46))
      return RACH;
    else
      return TSC;
    break;
  }
  case VII:
    if ((burstFN % 51 <= 14) && (burstFN % 51 >= 12))
      return IDLE;
    else
      return TSC;
    break;
  case LOOPBACK:
    if ((burstFN % 51 <= 50) && (burstFN % 51 >=48))
      return IDLE;
    else
      return TSC;
    break;
  default:
    return OFF;
    break;
  }

}

void Transceiver::pullRadioVector()
{
  radioVector *rxBurst = NULL;
  rxBurst = (radioVector *) mReceiveFIFO->get();
  if (!rxBurst) return;

  GSM::Time theTime = rxBurst->time();

  for (unsigned int i = 0; i < mNumARFCNs; i++) {
    CorrType corrType = expectedCorrType(rxBurst->time(),i);
    if ((corrType == OFF) || (corrType == IDLE)) continue;
    radioVector *ARFCNVec = new radioVector(*(signalVector *)rxBurst,theTime,(int&)i);
    if (mMultipleARFCN) {
      multVector(*ARFCNVec,*mFrequencyShifter[mNumARFCNs-1-i]);
      signalVector *rcvVec = polyphaseResampleVector(*ARFCNVec,1,mOversamplingRate,decimationFilter);
      delete ARFCNVec;
      ARFCNVec = new radioVector(*rcvVec,theTime,(int&)i);
      delete rcvVec;
    }
    mDemodFIFO[i]->put(ARFCNVec);
  }

  delete rxBurst;
}

void Transceiver::start()
{

}

void Transceiver::reset()
{
  mTransmitPriorityQueue.clear();
  //mTransmitFIFO->clear();
  //mReceiveFIFO->clear();
}

void Transceiver::driveReceiveFIFO()
{
  mRadioInterface->driveReceiveRadio();

  pullRadioVector();
}


void Transceiver::driveTransmitFIFO()
{

  /**
      Features a carefully controlled latency mechanism, to
      assure that transmit packets arrive at the radio/USRP
      before they need to be transmitted.

      Deadline clock indicates the burst that needs to be
      pushed into the FIFO right NOW.  If transmit queue does
      not have a burst, stick in filler data.
  */


  RadioClock *radioClock = (mRadioInterface->getClock());

  if (mOn) {
    radioClock->wait(); // wait until clock updates
    while (radioClock->get() + mTransmitLatency > mTransmitDeadlineClock) {
      // if underrun, then we're not providing bursts to radio/USRP fast
      //   enough.  Need to increase latency by one GSM frame.
      if (mRadioInterface->isUnderrun()) {
        // only do latency update every 10 frames, so we don't over update
	if (radioClock->get() > mLatencyUpdateTime + GSM::Time(100,0)) {
	  mTransmitLatency = mTransmitLatency + GSM::Time(1,0);
	  mLatencyUpdateTime = radioClock->get();
	}
      }
      else {
        // if underrun hasn't occurred in the last sec (216 frames) drop
        //    transmit latency by a timeslot
	if (mTransmitLatency > GSM::Time(1,1)) {
            if (radioClock->get() > mLatencyUpdateTime + GSM::Time(216,0)) {
	    mTransmitLatency.decTN();
	    mLatencyUpdateTime = radioClock->get();
	  }
	}
      }
      // time to push burst to transmit FIFO
      pushRadioVector(mTransmitDeadlineClock);
      mTransmitDeadlineClock.incTN();
    }

  }
  // FIXME -- This should not be a hard spin.
  // But any delay here causes us to throw omni_thread_fatal.
  //else radioClock->wait();
}



void Transceiver::writeClockInterface()
{
	GSMTRX::get().onClock( (unsigned long long) (mTransmitDeadlineClock.FN()+2) );

  mLastClockUpdateTime = mTransmitDeadlineClock;

}

void *FIFOServiceLoopAdapter(Transceiver *transceiver)
{
  while (1) {
    //transceiver->driveReceiveFIFO();
    transceiver->driveTransmitFIFO();
    pthread_testcancel();
  }
  return NULL;
}

void *RFIFOServiceLoopAdapter(Transceiver *transceiver)
{
  bool isMulti = transceiver->multiARFCN();
  while (1) {
    transceiver->driveReceiveFIFO();
    if (!isMulti) transceiver->mDemodulators[0]->driveDemod(true);
    //transceiver->driveTransmitFIFO();
    pthread_testcancel();
  }
  return NULL;
}
