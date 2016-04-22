#ifndef _DEMODULATOR_H_
#define _DEMODULATOR_H_

#include "radioInterface.h"
#include "Interthread.h"
#include "GSMCommon.h"

#include <sys/types.h>

class Transceiver;

#define MAXMODULUS 102

/** Codes for burst types of received bursts*/
typedef enum {
  OFF,               ///< timeslot is off
  TSC,	       ///< timeslot should contain a normal burst
  RACH,	       ///< timeslot should contain an access burst
  IDLE	       ///< timeslot is an idle (or dummy) burst
} CorrType;

class Demodulator {

 private:

  int mCN;
  Transceiver *mTRX;
  RadioInterface *mRadioInterface;
  VectorFIFO *mDemodFIFO;
  double mEnergyThreshold;             ///< threshold to determine if received data is potentially a GSM burst
  GSM::Time    prevFalseDetectionTime; ///< last timestamp of a false energy detection
  GSM::Time    channelEstimateTime[8]; ///< last timestamp of each timeslot's channel estimate
  signalVector *channelResponse[8];    ///< most recent channel estimate of all timeslots
  float        SNRestimate[8];         ///< most recent SNR estimate of all timeslots
  signalVector *DFEForward[8];         ///< most recent DFE feedforward filter of all timeslots
  signalVector *DFEFeedback[8];        ///< most recent DFE feedback filter of all timeslots
  float        chanRespOffset[8];      ///< most recent timing offset, e.g. TOA, of all timeslots
  complex      chanRespAmplitude[8];   ///< most recent channel amplitude of all timeslots
  signalVector *gsmPulse;
  unsigned     mTSC;
  unsigned     mSamplesPerSymbol;

  unsigned     mMaxExpectedDelay;

  double rxFullScale;                     ///< full scale output to radio

  SoftVector* demodRadioVector(radioVector *rxBurst,
                              GSM::Time &wTime,
                              int &RSSI,
                              int &timingOffset);

 public:

  Demodulator(int wCN,
	      Transceiver *wTRX,
	      GSM::Time wStartTime);

  double getEnergyThreshold() {return mEnergyThreshold;}

  int          mNoiseFloorRSSI;
//protected:

  void driveDemod(bool wSingleARFCN = true);
 protected:
  friend void *DemodServiceLoopAdapter(Demodulator *);

};

void *DemodServiceLoopAdapter(Demodulator *);

#endif
