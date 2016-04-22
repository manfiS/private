#ifndef _GSMTRX_H_
#define _GSMTRX_H_

typedef void * GSMCTX;

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
gsmtrx_loglevel_t;

typedef void (*gsmtrx_log_callback_t)( gsmtrx_loglevel_t, const char * );

typedef struct {
  gsmtrx_log_callback_t logger;
}
gsmtrx_callbacks_t;

GSMCTX gsmtrx_create( gsmtrx_callbacks_t *callbacks );
void gsmtrx_log( gsmtrx_loglevel_t level, const char *fmt, ... );
bool gsmtrx_init( GSMCTX ctx, unsigned int nARFCN = 1, const char *deviceArgs = "" );
void gsmtrx_start( GSMCTX ctx );
void gsmtrx_wait( GSMCTX ctx );
void gsmtrx_stop( GSMCTX ctx );
void gsmtrx_destroy( GSMCTX ctx );

#endif
