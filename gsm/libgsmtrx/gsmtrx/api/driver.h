#ifndef _DRIVER_H_
#define _DRIVER_H_

#include "Transceiver.h"
#include "burst.h"

class Driver : public Transceiver {
public:
  Driver( int wSamplesPerSymbol,
        GSM::Time wTransmitLatency,
        RadioInterface *wRadioInterface,
        unsigned int wNumARFCNs,
        unsigned int wOversamplingRate,
        bool wLoadTest);

  void powerOff();
  void powerOn();
  void setMaxDelay( int maxDelay );
  void setTrxGain( int gain );
  void setPower( unsigned int ARFCN, int dB );
  void setPowerAttenuation( unsigned int ARFCN, int dB );
  void setFrequencyOffset( unsigned int ARFCN, int tuneVoltage );
  int  getNoiseRSSI();
  bool tuneRx( unsigned int ARFCN, int freqKhz );
  bool tuneTx( unsigned int ARFCN, int freqKhz );
  void setTSC( unsigned int ARFCN, int TSC );
  void setHandover( unsigned int ARFCN, int timeslot, bool status );
  void setSlot( unsigned int ARFCN, int timeslot, int corrCode );
  int  runDeviceCommand( std::string cmd );

  void injectBurst( int ARFCN, GSM::Burst burst );
};

#endif
