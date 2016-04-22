#ifndef _LOGGER_H_
#define _LOGGER_H_

#include "burst.h"
#include "rach.h"

typedef enum {
  LOG_EMERG = 0, //  system is unusable
  LOG_ALERT,     //  action must be taken immediately
  LOG_CRIT,      //  critical conditions
  LOG_ERR,       //  error conditions
  LOG_WARNING,   //  warning conditions
  LOG_NOTICE,    //  normal, but significant, condition
  LOG_INFO,      //  informational message
  LOG_DEBUG      //  debug-level message
}
LOG_LEVEL;

class EventReceiver {
public:
  virtual void onLogMessage( LOG_LEVEL level, std::string message ) = 0;

  virtual void onClock( unsigned long long clock ){};
  virtual void onBurst( int channel, GSM::Burst burst ){};
  virtual void onL1RACHBurst( GSM::L1::RACHBurst burst ){};
};

#endif
