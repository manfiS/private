#include "gsmtrx.h"
#include "Transceiver.h"
#include <time.h>
#include <stdarg.h>

typedef struct {
  volatile bool         running;
  RadioDevice          *dev;
  RadioInterface       *radio;
  Transceiver          *trx;

  gsmtrx_callbacks_t    callbacks;
}
gsmtrx_t;

static gsmtrx_t *__instance = NULL;
static Mutex     __mutex;

void gsmtrx_log( gsmtrx_loglevel_t level, const char *fmt, ... ) {
  __mutex.lock();

  if( __instance && __instance->callbacks.logger ) {

    char 	buffer[0xFF] = {0};
	  va_list ap;

		va_start( ap, fmt );
			vsnprintf( buffer, 0xFF, fmt, ap );
		va_end(ap);

    strcat( buffer, "\n" );
    __instance->callbacks.logger( level, buffer );
  }

  __mutex.unlock();
}

GSMCTX gsmtrx_create( gsmtrx_callbacks_t *callbacks ) {
  if( __instance != NULL ){
    fprintf( stderr, "Another GSMCTX instance was previously created.\n" );
    exit(1);
  }

  __instance = new gsmtrx_t;

  __instance->running = false;
  __instance->dev     = NULL;
  __instance->radio   = NULL;
  __instance->trx     = NULL;

  memcpy( &__instance->callbacks, callbacks, sizeof(gsmtrx_callbacks_t) );

  return __instance;
}

bool gsmtrx_init( GSMCTX ctx, unsigned int nARFCN /* = 1 */, const char *deviceArgs /* = "" */ ) {
  gsmtrx_t *obj = (gsmtrx_t *)ctx;

  // Device specific global initialization
  RadioDevice::staticInit();

  srandom(time(NULL));

  int mOversamplingRate = 1;
  switch(nARFCN) {
    case 1:
      mOversamplingRate = 1;
    break;
    case 2:
      mOversamplingRate = 6;
    break;
    case 3:
      mOversamplingRate = 8;
    break;
    case 4:
      mOversamplingRate = 12;
    break;
    case 5:
      mOversamplingRate = 16;
    break;
    default:
      gsmtrx_log( LOG_ALERT, "Invalid nARFCN" );
      return false;
    break;
  }

  obj->dev = RadioDevice::make(mOversamplingRate);
  if (!obj->dev->open(deviceArgs)) {
    gsmtrx_log( LOG_ALERT, "Transceiver exiting..." );
    return false;
  }

  obj->radio = new RadioInterface( obj->dev, 3, SAMPSPERSYM, SAMPSPERSYM*mOversamplingRate, false, nARFCN );
  obj->trx = new Transceiver( 5700, "127.0.0.1",SAMPSPERSYM,GSM::Time(2,0),obj->radio,nARFCN,mOversamplingRate,false);
  obj->trx->receiveFIFO(obj->radio->receiveFIFO());

  return true;
}

void gsmtrx_start( GSMCTX ctx ) {
  gsmtrx_t *obj = (gsmtrx_t *)ctx;

  obj->running = true;
  obj->trx->start();
}

void gsmtrx_wait( GSMCTX ctx ) {
  gsmtrx_t *obj = (gsmtrx_t *)ctx;

  while(obj->running) {
    sleep(1);
  }
}

void gsmtrx_stop( GSMCTX ctx ) {
  gsmtrx_t *obj = (gsmtrx_t *)ctx;

  obj->running = false;
}

void gsmtrx_destroy( GSMCTX ctx ) {
  gsmtrx_t *obj = (gsmtrx_t *)ctx;

  delete obj->trx;
  delete obj;
}
