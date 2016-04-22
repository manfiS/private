#ifndef _BURST_H_
#define _BURST_H_

#include "GSMTransfer.h"

// number of symbols per slot, not counting guard periods
#define SYMBOLS_PER_SLOT 148
// maximum unified repeat period
#define MAX_MODULUS ( 51 * 26 * 4 )

namespace GSM {

class Burst {
public:

  GSM::Time time;
  int       rssi;
  int       toa;
  char      buffer[SYMBOLS_PER_SLOT];

  Burst( GSM::Time t, int rssi, int toa, SoftVector& v );

  std::string to_s( bool full = false );
  BitVector toBitVector();
};

}

#endif
