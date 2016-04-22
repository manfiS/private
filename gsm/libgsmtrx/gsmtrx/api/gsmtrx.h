#ifndef _GSMTRX_H_
#define _GSMTRX_H_

#include <stdarg.h>
#include <stdio.h>
#include <stdexcept>

#include <vector>

#include "eventreceiver.h"
#include "driver.h"
#include "singleton.hpp"
#include "burst.h"
#include "decoder.h"

#include "rach.h"
#include "Transceiver.h"

class GSMTRX : public Singleton<GSMTRX> {
  friend class Demodulator;
  friend class Transceiver;

  friend class GSM::L1::RACHDecoder;

private:

  EventReceiver  *m_receiver;
  unsigned int    m_nARFCN;
  int             m_OversamplingRate;
  int             m_minBurstRSSI;
  bool            m_running;
  RadioDevice    *m_device;
  RadioInterface *m_radio;
  Driver         *m_trx;

  Mutex             m_demux_lock;
  GSM::L1::Decoder *m_demux_table[8][MAX_MODULUS];

protected:

  void onClock( unsigned long long clock );
  void onBurst( int arfcn, GSM::Burst burst );
  // TODO: Split handlers by level?
  void onL1RACHBurst( GSM::L1::RACHBurst burst );

public:

  struct Configuration {
    int rx_frequency; // Khz
    int tx_frequency; // Khz
    int tsc;
    int max_delay;
    int gain;
    int power;
  };

  GSMTRX( EventReceiver *receiver, int minBurstRSSI = -90, unsigned int nARFCN = 1, std::string deviceArgs = "" );
  virtual ~GSMTRX();

  void start();
  void stop();
  void join();

  void setConfiguration( Configuration conf );
  void setChannelCombinations( std::vector<Transceiver::ChannelCombination> combos );

  void log( LOG_LEVEL level, const char *fmt, ... );

  inline Driver& driver() {
    return *m_trx;
  }

  void installDecoder(GSM::L1::Decoder* decoder);
};

#define LOG(LEVEL,FMT,...) GSMTRX::get().log( LOG_##LEVEL, FMT, ## __VA_ARGS__ )

#endif
