#include "driver.h"
#include "gsmtrx.h"

#define CONTROL(CODE) \
  mControlLock.lock(); \
  writeClockInterface(); \
  CODE \
  mControlLock.unlock();

Driver::Driver( int wSamplesPerSymbol,
      GSM::Time wTransmitLatency,
      RadioInterface *wRadioInterface,
      unsigned int wNumARFCNs,
      unsigned int wOversamplingRate,
      bool wLoadTest) : Transceiver( wSamplesPerSymbol, wTransmitLatency, wRadioInterface, wNumARFCNs, wOversamplingRate, wLoadTest ) {

}

void Driver::powerOff() { CONTROL(

)}

void Driver::powerOn() { CONTROL(
  // turn on transmitter/demod
  if( mTxFreq && mRxFreq ) {
    if (!mOn) {
      // Prepare for thread start
      mPower = -20;
      mRadioInterface->start();

      // Start radio interface threads.
      writeClockInterface();
      generateRACHSequence(*gsmPulse,mSamplesPerSymbol);

      mRFIFOServiceLoopThread->start((void * (*)(void*))RFIFOServiceLoopAdapter,(void*) this);
      mFIFOServiceLoopThread->start((void * (*)(void*))FIFOServiceLoopAdapter,(void*) this);

      for( unsigned int i = 0; i < mNumARFCNs; i++ ) {
        ThreadStruct *cs = new ThreadStruct;
        cs->trx = this;
        cs->CN = i;

        Demodulator *demod = new Demodulator(i,this,mStartTime);
        mDemodulators[i] = demod;
        if(mMultipleARFCN){
          mDemodServiceLoopThread[i]->start((void * (*)(void*))DemodServiceLoopAdapter,(void*) demod);
        }
      }

      mOn = true;
    }
  }
  else {
    LOG(WARNING, "Requested power on but TX or RX frequency is not set." );
  }
)}

void Driver::setMaxDelay( int maxDelay ) { CONTROL(
  mMaxExpectedDelay = maxDelay; // 1 GSM symbol is approx. 1 km
)}

void Driver::setTrxGain( int gain ) { CONTROL(
  mRadioInterface->setRxGain(gain);
)}

void Driver::setPowerAttenuation( unsigned int ARFCN, int dB ) { CONTROL(
  if( mOn ){
    if (ARFCN==0) {
      mRadioInterface->setPowerAttenuation(mPower + dB);
    }
  }
)}

void Driver::setPower( unsigned int ARFCN, int dB ) { CONTROL(
  if( mOn ){
    if (ARFCN==0) {
      mPower = dB;
      mRadioInterface->setPowerAttenuation(mPower);
    }
  }
)}

void Driver::setFrequencyOffset( unsigned int ARFCN, int tuneVoltage ) { CONTROL(
  if( mOn ){
    if (ARFCN==0) {
      mRadioInterface->setVCTCXO(tuneVoltage);
    }
  }
)}

int Driver::getNoiseRSSI() { CONTROL(
  return mOn ? mDemodulators[0]->mNoiseFloorRSSI : 0;
)}

bool Driver::tuneRx( unsigned int ARFCN, int freqKhz ) { CONTROL(
  if( ARFCN == 0 ){
    mRxFreq = freqKhz*1.0e3+mFreqOffset;
    return !mRadioInterface->tuneRx(mRxFreq,128);
  }

  return true;
)}

bool Driver::tuneTx( unsigned int ARFCN, int freqKhz ) { CONTROL(
  if( ARFCN == 0 ){
    mTxFreq = freqKhz*1.0e3+mFreqOffset;
    return !mRadioInterface->tuneTx(mTxFreq,128);
  }

  return true;
)}

void Driver::setTSC( unsigned int ARFCN, int TSC ) { CONTROL(
  if( !mOn && ARFCN == 0 ){
    mTSC = TSC;
    generateMidamble(*gsmPulse,mSamplesPerSymbol,TSC);
  }
)}

void Driver::setHandover( unsigned int ARFCN, int timeslot, bool status ) { CONTROL(
  if( ARFCN >= 0 && ARFCN <= MAXARFCN && timeslot >= 0 && timeslot <= 7 ){
    mHandoverActive[ARFCN][timeslot] = status;
  }
)}

void Driver::setSlot( unsigned int ARFCN, int timeslot, int corrCode ) { CONTROL(
  if( ARFCN >= 0 && ARFCN <= MAXARFCN && timeslot >= 0 && timeslot <= 7 ){
    mChanType[ARFCN][timeslot] = (ChannelCombination) corrCode;
    setModulus(ARFCN,timeslot);
  }
)}

int Driver::runDeviceCommand( std::string cmd ) { CONTROL(
  return mRadioInterface->getDevice()->runCustom(cmd);
)}

void Driver::injectBurst( int ARFCN, GSM::Burst burst ) {
  ScopedLock lock(mTransmitPriorityQueueLock);

  // Magic flag says this is a filler burst.
  int timeSlot   = burst.time.TN();
  int fillerFlag = timeSlot & SET_FILLER_FRAME;
  timeSlot = timeSlot & 0x7;

  // periodically update GSM core clock
  if( mTransmitDeadlineClock > mLastClockUpdateTime + GSM::Time(216,0) )
    writeClockInterface();

  // DAVID COMMENT: You can't tell from the name, but fixRadioVector
  // resamples and frequency-shifts to simulate multiple transceivers
  // in software.
  //
  // DAVID COMMENT: It would be more efficient to apply the upsampling and frequency-shifting
  // later in the transmission process, just before the ARFCNs are summed for transmission.
  //
  BitVector bv = burst.toBitVector();
  radioVector *newVec = fixRadioVector( bv, burst.rssi, burst.time, ARFCN );

  if( fillerFlag ) {
  	setFiller( newVec, false, true );
  }
  else {
    mTransmitPriorityQueue.write(newVec);
  }
}
