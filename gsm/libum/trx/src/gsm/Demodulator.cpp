#include <stdio.h>
#include "Transceiver.h"
#include "Demodulator.h"
#include <gsmtrx.h>

Demodulator::Demodulator(int wCN,
			 Transceiver *wTRX,
			 GSM::Time wStartTime)
{

  assert(wTRX);

  mCN = wCN;
  mTRX = wTRX;
  mRadioInterface = mTRX->radioInterface();
  mTRXDataSocket = mTRX->dataSocket(mCN);
  mSamplesPerSymbol = mTRX->samplesPerSymbol();
  mDemodFIFO = mTRX->demodFIFO(mCN);
  //signalVector *gsmPulse = mTRX->GSMPulse();
  mTSC = mTRX->getTSC();

  rxFullScale = mRadioInterface->fullScaleOutputValue();
  mNoiseFloorRSSI = 0;

  gsmtrx_log( LOG_DEBUG, "Creating demodulator for CN %d with TSC %d", mCN, mTSC );

  for (unsigned i = 0; i < 8; i++) {
      channelResponse[i] = NULL;
      DFEForward[i] = NULL;
      DFEFeedback[i] = NULL;
      channelEstimateTime[i] = wStartTime;
      mEnergyThreshold = 7.07;
  }

  prevFalseDetectionTime = wStartTime;

}

void Demodulator::driveDemod(bool wSingleARFCN)
{
  radioVector *demodBurst = NULL;
  SoftVector *rxBurst = NULL;
  int RSSI;
  int TOA;  // in 1/256 of a symbol
  GSM::Time burstTime;

  //RadioClock *radioClock = (mRadioInterface->getClock());
  //radioClock->wait();


  demodBurst = mDemodFIFO->get();
  if (!wSingleARFCN) {
    while  (!demodBurst) {
      RadioClock *radioClock = (mRadioInterface->getClock());
      radioClock->wait();
      demodBurst = mDemodFIFO->get();
    }
  }
  else {
    if (!demodBurst) return;
  }

  mMaxExpectedDelay = mTRX->maxDelay();

  rxBurst = demodRadioVector(demodBurst,burstTime,RSSI,TOA);

  if (rxBurst) {
    char burstString[gSlotLen+10];
    burstString[0] = burstTime.TN();
    for (int i = 0; i < 4; i++)
      burstString[1+i] = (burstTime.FN() >> ((3-i)*8)) & 0x0ff;
    burstString[5] = RSSI;
    burstString[6] = (TOA >> 8) & 0x0ff;
    burstString[7] = TOA & 0x0ff;
    SoftVector::iterator burstItr = rxBurst->begin();

    for (unsigned int i = 0; i < gSlotLen; i++) {
      burstString[8+i] =(char) round((*burstItr++)*255.0);
    }
    burstString[gSlotLen+9] = '\0';
    delete rxBurst;

    mTRXDataSocket->write(burstString,gSlotLen+10);
  }

}

SoftVector *Demodulator::demodRadioVector(radioVector *rxBurst,
					 GSM::Time &wTime,
					 int &RSSI,
					 int &timingOffset)
{

  bool needDFE = (mMaxExpectedDelay > 1);

  int timeslot = rxBurst->time().TN();

  CorrType corrType = mTRX->expectedCorrType(rxBurst->time(),mCN);

  if ((corrType==OFF) || (corrType==IDLE)) {
    delete rxBurst;
    return NULL;
  }

  // check to see if received burst has sufficient
  signalVector *vectorBurst = rxBurst;
  complex amplitude = 0.0;
  float TOA = 0.0;
  float avgPwr = 0.0;
  /*if (!energyDetect(*vectorBurst,20*mSamplesPerSymbol,mEnergyThreshold,&avgPwr)) {
     double framesElapsed = rxBurst->time()-prevFalseDetectionTime;
     if (framesElapsed > 50) {  // if we haven't had any false detections for a while, lower threshold
	//mEnergyThreshold -= 1.0;
        prevFalseDetectionTime = rxBurst->time();
     }
     delete rxBurst;
     return NULL;
  }*/

  // run the proper correlator
  bool success = false;
  if (corrType==TSC) {
    signalVector *channelResp;
    //double framesElapsed = rxBurst->time()-channelEstimateTime[timeslot];
    bool estimateChannel = false;
    //if ((framesElapsed > 50) || (channelResponse[timeslot]==NULL))
    {
	if (channelResponse[timeslot]) delete channelResponse[timeslot];
        if (DFEForward[timeslot]) delete DFEForward[timeslot];
        if (DFEFeedback[timeslot]) delete DFEFeedback[timeslot];
        channelResponse[timeslot] = NULL;
        DFEForward[timeslot] = NULL;
        DFEFeedback[timeslot] = NULL;
	estimateChannel = true;
    }
    estimateChannel = true;
    if (!needDFE) estimateChannel = false;
    float chanOffset;

    success = analyzeTrafficBurst(*vectorBurst,
				  mTSC,
				  3.0,
				  mSamplesPerSymbol,
				  &amplitude,
				  &TOA,
				  mMaxExpectedDelay,
                                  estimateChannel,
				  &channelResp,
				  &chanOffset);

    if (success) {
      //mEnergyThreshold -= 0.1F;
      if (mEnergyThreshold < 0.0) mEnergyThreshold = 0.0;
      SNRestimate[timeslot] = amplitude.norm2()/(mEnergyThreshold*mEnergyThreshold+1.0); // this is not highly accurate
      if (estimateChannel) {
         channelResponse[timeslot] = channelResp;
       	 chanRespOffset[timeslot] = chanOffset;
         chanRespAmplitude[timeslot] = amplitude;
	 scaleVector(*channelResp, complex(1.0,0.0)/amplitude);
         designDFE(*channelResp, SNRestimate[timeslot], 7, &DFEForward[timeslot], &DFEFeedback[timeslot]);
         channelEstimateTime[timeslot] = rxBurst->time();
      }
    }
    else {
      double framesElapsed = rxBurst->time()-prevFalseDetectionTime;
      //mEnergyThreshold += 0.1F*exp(-framesElapsed);
      prevFalseDetectionTime = rxBurst->time();
      channelResponse[timeslot] = NULL;
    }
  }
  else {
    // RACH burst
    success = detectRACHBurst(*vectorBurst,
			      3.0,  // detection threshold
			      mSamplesPerSymbol,
			      &amplitude,
			      &TOA);
    if (success) {
      //mEnergyThreshold -= 0.1F;
      if (mEnergyThreshold < 0.0) mEnergyThreshold = 0.0;
      channelResponse[timeslot] = NULL;
    }
    else {
      //double framesElapsed = rxBurst->time()-prevFalseDetectionTime;
      //mEnergyThreshold += 0.1F*exp(-framesElapsed);
      prevFalseDetectionTime = rxBurst->time();
      float avgPwr;
      energyDetect(*vectorBurst,20*mSamplesPerSymbol,0.0,&avgPwr);
      mNoiseFloorRSSI = (int) floor(20.0*log10(rxFullScale/sqrt(avgPwr)));
    }
  }

  // demodulate burst
  SoftVector *burst = NULL;
  if ((rxBurst) && (success)) {
    if ((corrType==RACH) || (!needDFE)) {
      burst = demodulateBurst(*vectorBurst,
			      *gsmPulse,
			      mSamplesPerSymbol,
			      amplitude,TOA);
    }
    else { // TSC
      scaleVector(*vectorBurst,complex(1.0,0.0)/amplitude);
      burst = equalizeBurst(*vectorBurst,
			    TOA-chanRespOffset[timeslot],
			    mSamplesPerSymbol,
			    *DFEForward[timeslot],
			    *DFEFeedback[timeslot]);
    }
    wTime = rxBurst->time();
    // FIXME:  what is full scale for the USRP?  we get more that 12 bits of resolution...
    RSSI = (int) floor(20.0*log10(rxFullScale/amplitude.abs()));
    timingOffset = (int) round(TOA*256.0/mSamplesPerSymbol);
  }

  delete rxBurst;

  return burst;
}

void *DemodServiceLoopAdapter(Demodulator *demodulator)
{
  while(1) {
    demodulator->driveDemod(false);
    pthread_testcancel();
  }
  return NULL;
}
