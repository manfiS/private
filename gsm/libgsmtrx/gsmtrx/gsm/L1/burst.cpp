#include "burst.h"
#include <math.h>
#include <stdio.h>

namespace GSM {

Burst::Burst( GSM::Time t, int rssi, int toa, SoftVector& v ) :
  time(t),
  rssi(-rssi),
  toa(toa)
{
  memset( buffer, 0x00, SYMBOLS_PER_SLOT );

  SoftVector::iterator iter = v.begin();
  for( unsigned int i = 0; i < SYMBOLS_PER_SLOT; ++i ) {
    buffer[i] = (char)round((*iter++)*255.0);
  }
}

std::string Burst::to_s( bool full /* = false */ ) {
  char s[0xFFF] = {0};
  int n = sprintf( s, "[GSM::BURST] TS=%d FN=%d RSSI=%d TOA=%d", time.TN(), time.FN(), rssi, toa );
  if( full && n ){
    strcat( s, "\n" );
    char *p = s + n + 1;

    for( int i = 0; i < SYMBOLS_PER_SLOT; ++i ){
      sprintf( p, "%02x", buffer[i] );
      p += 2;
    }

    strcat( p, "\n" );
  }

  return std::string(s);
}

BitVector Burst::toBitVector() {
  BitVector bv(SYMBOLS_PER_SLOT);
  BitVector::iterator itr = bv.begin();
  char *p = &buffer[0];

  while( itr < bv.end() )
    *itr++ = *p++;

  return bv;
}

}
