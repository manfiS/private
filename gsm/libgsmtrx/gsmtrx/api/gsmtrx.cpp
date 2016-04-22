#include "gsmtrx.h"

#include "rach.h"

GSMTRX::GSMTRX( EventReceiver *receiver, int minBurstRSSI, unsigned int nARFCN, std::string deviceArgs ) : Singleton(this),
m_receiver(receiver),
m_nARFCN(nARFCN),
m_running(false),
m_device(NULL),
m_radio(NULL),
m_trx(NULL),
m_OversamplingRate(1),
m_minBurstRSSI(minBurstRSSI)
  {
  // Device specific global initialization
  RadioDevice::staticInit();

  for( int i = 0; i < 8; ++i ) {
    for( int j = 0; j < MAX_MODULUS; ++j ){
      m_demux_table[i][j] = NULL;
    }
  }

  // install decoders
  installDecoder( new GSM::L1::RACHDecoder( gRACHC5Mapping, 0 ) );

  srandom(time(NULL));

  switch(m_nARFCN) {
    case 1:
      m_OversamplingRate = 1;
    break;
    case 2:
      m_OversamplingRate = 6;
    break;
    case 3:
      m_OversamplingRate = 8;
    break;
    case 4:
      m_OversamplingRate = 12;
    break;
    case 5:
      m_OversamplingRate = 16;
    break;
    default:
      throw std::runtime_error( "Invalid nARFCN parameter." );
    break;
  }

  m_device = RadioDevice::make(m_OversamplingRate);
  if( !m_device->open(deviceArgs) ) {
    throw std::runtime_error( "Could not open device." );
  }

  m_radio = new RadioInterface( m_device, 3, SAMPSPERSYM, SAMPSPERSYM*m_OversamplingRate, false, m_nARFCN );
  m_trx   = new Driver( SAMPSPERSYM, GSM::Time(2,0), m_radio, m_nARFCN, m_OversamplingRate, false );

  m_trx->receiveFIFO(m_radio->receiveFIFO());
}

void GSMTRX::log( LOG_LEVEL level, const char *fmt, ... ) {
  char buffer[0xFF] = {0};
  va_list ap;

  va_start( ap, fmt );
    vsnprintf( buffer, 0xFF, fmt, ap );
  va_end(ap);

  strcat( buffer, "\n" );

  m_receiver->onLogMessage( level, buffer );
}

void GSMTRX::onClock( unsigned long long clock ) {
  m_receiver->onClock(clock);
}

void GSMTRX::onBurst( int arfcn, GSM::Burst burst ) {
  if( burst.rssi < m_minBurstRSSI ){
    LOG( INFO, "Dropping GSM burst with RSSI %d.", burst.rssi );
    return;
  }

  LOG( DEBUG, "Processing %s", burst.to_s().c_str() );

	uint32_t FN = burst.time.FN() % MAX_MODULUS;
	unsigned TN = burst.time.TN();

  L1::Decoder *decoder = NULL;

	m_demux_lock.lock();
	decoder = m_demux_table[TN][FN];
  m_demux_lock.unlock();

	if( decoder == NULL ) {
		LOG( DEBUG, "Unconfigured TDMA position %s", burst.to_s().c_str() );
    m_receiver->onBurst( arfcn, burst );
    return;
	}

  float data[SYMBOLS_PER_SLOT] = {0.0};
  for( unsigned i = 0; i < SYMBOLS_PER_SLOT; ++i ){
    data[i] = burst.buffer[i] / 256.0F;
  }

	decoder->writeLowSideRx( RxBurst( data, burst.time, burst.toa / 256.0F, burst.rssi ) );
}

void GSMTRX::onL1RACHBurst( GSM::L1::RACHBurst burst ) {
  if( burst.rssi < m_minBurstRSSI ){
    LOG( INFO, "Dropping L1::RACH burst with RSSI %d.", burst.rssi );
    return;
  }

  m_receiver->onL1RACHBurst( burst );
}

GSMTRX::~GSMTRX() {
  if( m_trx )
    delete m_trx;
}

void GSMTRX::start() {
  if( m_running == false ){
    m_running = true;
    m_trx->start();
  }
  else {
    LOG( WARNING, "GSMTRX already started." );
  }
}

void GSMTRX::stop() {
  if( m_running == true ){
    m_running = false;
  }
  else {
    LOG( WARNING, "GSMTRX not started." );
  }
}

void GSMTRX::join() {
  while(m_running){
    sleep(1);
  }
}

void GSMTRX::installDecoder(GSM::L1::Decoder* decoder) {
  unsigned TN = decoder->TN();
  const TDMAMapping& mapping = decoder->mapping();

  LOG( DEBUG, "Installing L1::Decoder TN: %u REPLEN: %u.\n", TN, mapping.repeatLength() );

  m_demux_lock.lock();
  for( unsigned i = 0; i < mapping.numFrames(); ++i ) {
    unsigned FN = mapping.frameMapping(i);
    while( FN < MAX_MODULUS ) {
      m_demux_table[TN][FN] = decoder;
      FN += mapping.repeatLength();
    }
  }
  m_demux_lock.unlock();
}

void GSMTRX::setConfiguration( Configuration conf ) {
  driver().powerOff();

  driver().tuneRx( 0, conf.rx_frequency );
  driver().tuneTx( 0, conf.tx_frequency );
  driver().setTSC( 0, conf.tsc );
  driver().setMaxDelay( conf.max_delay );
  driver().setTrxGain( conf.gain );

  driver().powerOn();

  driver().setPower( 0, conf.power );

  // Tell the TDMA handler what we're gonna receive and on which channel.
  std::vector<Transceiver::ChannelCombination> channels;

  channels.push_back( Transceiver::V ); // FCCH+SCH+CCCH+BCCH+SDCCH/4+SACCH/4, uplink RACH+SDCCH/4
  channels.push_back( Transceiver::I ); // TCH/FS
  channels.push_back( Transceiver::I ); // TCH/FS
  channels.push_back( Transceiver::I ); // TCH/FS
  channels.push_back( Transceiver::I ); // TCH/FS
  channels.push_back( Transceiver::I ); // TCH/FS
  channels.push_back( Transceiver::I ); // TCH/FS
  channels.push_back( Transceiver::I ); // TCH/FS

  setChannelCombinations(channels);

  driver().setPower( 0, conf.power );
}

void GSMTRX::setChannelCombinations( std::vector<Transceiver::ChannelCombination> combos ) {
  for( int timeslot = 0; timeslot < combos.size(); ++timeslot ) {
    driver().setSlot( 0, timeslot, combos[timeslot] );
    driver().setHandover( 0, timeslot, combos[timeslot] );
  }
}
