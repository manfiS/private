#include <stdlib.h>
#include <signal.h>
#include <iostream>
#include <stdio.h>
#include "gsmtrx.h"

using namespace std;

GSMCTX gsm;

void log_callback( gsmtrx_loglevel_t level, const char *msg ) {
  if( level < LOG_NOTICE )
    printf( "%s", msg );
}

gsmtrx_callbacks_t callbacks = {
  .logger = log_callback
};

static void ctrlCHandler(int signo) {
   cout << "Received shutdown signal" << endl;;
   gsmtrx_stop( gsm );
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

  gsm = gsmtrx_create(&callbacks);

  if( gsmtrx_init( gsm ) == false ){
    cerr << "Couldn't install initialize context." << endl;
    exit(1);
  }

  gsmtrx_start( gsm );

  gsmtrx_wait( gsm );

  cout << "Shutting down transceiver..." << endl;

  gsmtrx_destroy( gsm );
}
