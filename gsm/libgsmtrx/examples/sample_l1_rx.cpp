#include <signal.h>
#include <iostream>
#include "gsmtrx.h"

using namespace std;

class LogReceiver : public EventReceiver {
  std::string l2s(LOG_LEVEL level){
    switch(level) {
      case LOG_EMERG:   return "EMERG";
      case LOG_ALERT:   return "ALERT";
      case LOG_CRIT:    return "CRIT";
      case LOG_ERR:     return "ERR";
      case LOG_WARNING: return "WARNING";
      case LOG_NOTICE:  return "NOTICE";
      case LOG_INFO:    return "INFO";
      case LOG_DEBUG:   return "DEBUG";
      default: return "???";
    }
  }

public :

  virtual void onClock( unsigned long long clock ){};
  virtual void onBurst( int channel, GSM::Burst burst ){};
  virtual void onL1RACHBurst( GSM::L1::RACHBurst burst ){};

  void onLogMessage( LOG_LEVEL level, std::string message ) {
    if( level <= LOG_CRIT )
      cout << "[" << l2s(level) << "] " << message;
  }
};

class MyReceiver : public LogReceiver {

public:
  void onClock( unsigned long long clock ){}

  // Undecoded bursts.
  void onBurst( int arfcn, GSM::Burst burst ) {
    cout << burst.to_s() << "\n";
  }

  void onL1RACHBurst( GSM::L1::RACHBurst burst ){
    cout << burst.to_s() << "\n";
  }
};

static void ctrlCHandler(int signo) {
   GSMTRX::get().stop();
}

int main(int argc, char *argv[])
{
  if( signal( SIGINT, ctrlCHandler ) == SIG_ERR ) {
    cerr << "Couldn't install signal handler for SIGINT" << endl;
    exit(1);
  }

  if( signal( SIGTERM, ctrlCHandler ) == SIG_ERR ) {
    cerr << "Couldn't install signal handler for SIGTERM" << endl;
    exit(1);
  }

  GSMTRX::Configuration config = {
      .rx_frequency = 885200,
      .tx_frequency = 930200,
      .tsc          = 2,
      .max_delay    = 4,
      .gain         = 5,
      .power        = 35
  };
  GSMTRX trx( new MyReceiver );

  trx.start();

  trx.setConfiguration(config);

  trx.join();
}
