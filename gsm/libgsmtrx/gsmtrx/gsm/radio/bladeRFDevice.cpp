/*
* Copyright 2014 Free Software Foundation, Inc.
*
* This software is distributed under the terms of the GNU Affero Public License.
* See the COPYING file in the main directory for details.
*
* This use of this software may be subject to additional restrictions.
* See the LEGAL file in the main directory for details.

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU Affero General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU Affero General Public License for more details.

	You should have received a copy of the GNU Affero General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/


#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "Threads.h"
#include "bladeRFDevice.h"

#include <gsmtrx.h>


#define MIN_OVERSAMPLING 4

 /* Stream defaults */
#define DEFAULT_STREAM_RX_XFERS        1
#define DEFAULT_STREAM_RX_BUFFERS      8
#define DEFAULT_STREAM_TX_XFERS        1
#define DEFAULT_STREAM_TX_BUFFERS      8
#define DEFAULT_STREAM_SAMPLES      2048
#define DEFAULT_STREAM_TIMEOUT       500

#define BANDWIDTH 1500000

#define TX_GAIN1 BLADERF_TXVGA1_GAIN_MAX
#define MAX_RX_DC_OFFSET 63
#define MAX_TX_DC_OFFSET 31
#define SHIFT_RX_DC 5
#define SHIFT_TX_DC 4

#define RX_OFFSET_ERROR 10
#define RX_OFFSET_COEF 1.5
#define RX_AVERAGE_DAMPING 1024

#if (RX_AVERAGE_DAMPING < 2)
#error RX_AVERAGE_DAMPING must be at least 2
#endif

//#define SHOW_COUNTERS 4333

using namespace std;

bladeRFDevice::bladeRFDevice(int oversampling, bool skipRx)
    : skipRx(skipRx), rxGain(0.0)
{
    LOG(INFO, "Creating bladeRF device..." );

    sps = oversampling;
}

bool bladeRFDevice::open(const std::string &args, bool)
{
    ScopedLock myLock(writeLock);

    struct bladerf_version ver;
    bladerf_version(&ver);

    LOG(INFO, "bladeRF library version %d.%d.%d ( %s ).", ver.major, ver.minor, ver.patch, ver.describe );

    int status = bladerf_open(&bdev, args.c_str());
    if (status < 0) {
        LOG(ALERT, "Could not open bladeRF device: %s", bladerf_strerror(status) );
        return false;
    }
    char buf[34];
    bladerf_get_serial(bdev, buf);
    status = bladerf_fw_version(bdev, &ver);
    if (status < 0) {
        LOG(WARNING, "Opened bladeRF serial=%s firmware version error: %s", buf, bladerf_strerror(status) );
    }
    else {
        LOG(NOTICE, "Opened bladeRF serial=%s firmware version %d.%d.%d ( %s ).",
          buf,
          ver.major,
          ver.minor,
          ver.patch,
          ver.describe );
    }

    const char* fpgaName = "none";
    bladerf_fpga_size bfs;
    if (!(status = bladerf_get_fpga_size(bdev, &bfs))) {
        fpgaName = (bfs == BLADERF_FPGA_40KLE) ? "hostedx40.rbf" : "hostedx115.rbf";
#if 0
        status = bladerf_load_fpga(bdev, fpgaName);
        if (status < 0) {
            LOG(ALERT, "Error loading FPGA file %s: %s", fpgaName, bladerf_strerror(status) );
            return false;
        }
#endif

    } else {
        LOG(ALERT, "Error fetching FPGA size: %s", bladerf_strerror(status) );
        return false;
    }

    status = bladerf_fpga_version(bdev, &ver);
    if (status < 0) {
        LOG(WARNING, "bladeRF FPGA %s is loaded, version error: %s", fpgaName, bladerf_strerror(status) );
    }
    else {
        LOG(INFO, "bladeRF FPGA %s is loaded with version %d.%d.%d ( %s ).",
          fpgaName,
          ver.major,
          ver.minor,
          ver.patch,
          ver.describe );
    }

    uint32_t val = 0;
    bladerf_config_gpio_read(bdev, &val);
    val |= 0x10000; //enable timestamps, clears and resets everything on write
    bladerf_config_gpio_write(bdev, val);
    bladerf_config_gpio_read(bdev, &val);
    if (!(val & 0x10000)) {
        LOG(ALERT, "Could not enable timestamps" );
        return false;
    }

    LOG(INFO, "bladeRF timestamping enabled" );

    if (BLADERF_DEVICE_SPEED_SUPER != bladerf_device_speed(bdev)) {
        LOG(EMERG, "bladeRF transceiver only supports SuperSpeed mode at the moment." );
        return false;
    }

    struct bladerf_rational_rate rate, actual;
    rate.integer = (sps * 13e6) / 48;
    rate.num = (sps * 13e6) - rate.integer * 48;
    rate.den = 48;

    if ((status = bladerf_set_rational_sample_rate(bdev, BLADERF_MODULE_RX, &rate, &actual) < 0)) {
        LOG(ALERT, "Error setting RX sampling rate: %s", bladerf_strerror(status) );
        return false;
    }

    rate.integer = (sps * 13e6) / 48;
    rate.num = (sps * 13e6) - rate.integer * 48;
    rate.den = 48;
    if ((status = bladerf_set_rational_sample_rate(bdev, BLADERF_MODULE_TX, &rate, &actual)) < 0) {
        LOG(ALERT, "Error setting TX sampling rate: %s", bladerf_strerror(status) );
        return false;
    }

    unsigned int bw = 0;
    if ((status = bladerf_set_bandwidth(bdev, BLADERF_MODULE_RX, BANDWIDTH, &bw) < 0)) {
        LOG(ALERT, "Error setting RX bandwidth: %s", bladerf_strerror(status) );
    }

    if ((status = bladerf_set_bandwidth(bdev, BLADERF_MODULE_TX, BANDWIDTH, &bw) < 0)) {
        LOG(ALERT, "Error setting TX bandwidth: %s", bladerf_strerror(status) );
    }
    else {
        LOG(INFO, "Actual bandwidth %d", bw );
    }

    status = bladerf_sync_config(bdev,
        BLADERF_MODULE_RX,
        BLADERF_FORMAT_SC16_Q11,
        DEFAULT_STREAM_RX_BUFFERS,
        DEFAULT_STREAM_SAMPLES,
        DEFAULT_STREAM_RX_XFERS,
        DEFAULT_STREAM_TIMEOUT
    );

    if (status < 0)
        LOG(CRIT, "Failed to intialize RX sync handle: %s", bladerf_strerror(status) );

    status = bladerf_sync_config(bdev,
        BLADERF_MODULE_TX,
        BLADERF_FORMAT_SC16_Q11,
        DEFAULT_STREAM_TX_BUFFERS,
        DEFAULT_STREAM_SAMPLES,
        DEFAULT_STREAM_TX_XFERS,
        DEFAULT_STREAM_TIMEOUT
    );

    if (status < 0)
        LOG(CRIT, "Failed to intialize TX sync handle: %s", bladerf_strerror(status) );

    mRxGain1 = BLADERF_RXVGA1_GAIN_MAX;
    mDcCorrect = true;
    mRxMaxOffset = RX_OFFSET_ERROR * RX_AVERAGE_DAMPING;
    mRxCorrectionI = mRxCorrectionQ = MAX_RX_DC_OFFSET + 1;

    setRxOffsets( 0, 0 );
    setTxOffsets( 0, 0 );

    // Set initial gains to minimum, the transceiver will adjust them later
    setTxGain(minTxGain());
    setRxGain(minRxGain());

    samplesRead = 0;
    samplesWritten = 0;
    pulseMode = -1;
    rxShowInfo = 0;
    txShowInfo = 0;
    spammy = false;
    return true;
}


bool bladeRFDevice::start()
{
  LOG(NOTICE, "Starting bladeRF..." );
  if (!bdev) return false;

  writeLock.lock();

  int status;
  if ((status = bladerf_enable_module(bdev, BLADERF_MODULE_RX, 1)) < 0) {
      LOG(ALERT, "Error enabling RX: %s", bladerf_strerror(status) );
      return false;
  }

  if ((status = bladerf_enable_module(bdev, BLADERF_MODULE_TX, 1)) < 0) {
      LOG(ALERT, "Error enabling TX: %s", bladerf_strerror(status) );
      return false;
  }

  writeLock.unlock();

  data = new short[currDataSize];
  rxBufIndex = 0;
  rxBufCount = 0;
  rxConsumed = PKT_SAMPLES_USE;
  txBuffered = 0;
  rxTimestamp = initialReadTimestamp();
  txTimestamp = initialWriteTimestamp();
  rxResyncCandidate = 0;
  samplesRead = 0;
  samplesWritten = 0;
  mRxAverageI = 0;
  mRxAverageQ = 0;

  return true;
}

bool bladeRFDevice::stop()
{
  bladerf_enable_module(bdev, BLADERF_MODULE_RX, 0);
  bladerf_enable_module(bdev, BLADERF_MODULE_TX, 0);
  delete[] data;

  return true;
}

double bladeRFDevice::setTxGain(double dB)
{
    // RAD1 supports only attentuation
    dB += maxTxGain();

    if (dB > maxTxGain()) dB = maxTxGain();
    if (dB < minTxGain()) dB = minTxGain();

    writeLock.lock();
    bladerf_set_txvga1(bdev, TX_GAIN1);
    int status = bladerf_set_txvga2(bdev, dB);
    writeLock.unlock();
    if (status < 0) {
        LOG(ERR, "Error setting TX gain: %s", bladerf_strerror(status) );
    }

    return dB - maxTxGain();
}

double bladeRFDevice::setRxGain(double dB)
{
    if (dB > maxRxGain()) dB = maxRxGain();
    if (dB < minRxGain()) dB = minRxGain();

    writeLock.lock();
    mRxMaxOffset = (dB * RX_OFFSET_COEF + RX_OFFSET_ERROR) * RX_AVERAGE_DAMPING;
    bladerf_set_rxvga1(bdev, mRxGain1);
    int status = bladerf_set_rxvga2(bdev, dB);
    writeLock.unlock();
    if (status < 0) {
        LOG(ERR, "Error setting RX gain: %s", bladerf_strerror(status) );
    }

    rxGain = dB;

    return dB;
}


// NOTE: Assumes sequential reads
int bladeRFDevice::readSamples(short *buf, int len, bool *overrun,
        TIMESTAMP timestamp,
        bool *underrun,
        unsigned *RSSI)
{
    if (len <= 0 || !bdev) return 0;

    if (underrun) *underrun = false;
    if (overrun) *overrun = false;

    int rlen = 0;
    while (len > 0) {
        if (timestamp != rxTimestamp) {
            long long delta = (long long)(rxTimestamp - timestamp);

            if (delta > 0) {
                // Pad with zeros
                if (delta > len)
                    delta = len;
                memset(buf, 0, delta * sizeof(uint16_t) * 2);
                timestamp += delta;
                buf += (2 * delta);
                len -= delta;
                rlen += delta;
                if (len <= 0)
                    break;
            }
            else {
                // Discard some data
                delta = -delta;
                int rxAvail = PKT_SAMPLES_USE - rxConsumed;
                if (rxAvail > 0) {
                    if (delta > rxAvail)
                        delta = rxAvail;
                    rxConsumed += delta;
                    rxTimestamp += delta;
                    if (rxResyncCandidate)
                        rxResyncCandidate += delta;
                }
                if (overrun)
                    *overrun = true;
            }
        }
        int rxLen = PKT_SAMPLES_USE - rxConsumed;
        if (rxLen > 0) {
            if (rxLen > len)
                rxLen = len;
            memcpy(buf, &rxBuffer[rxBufIndex].samples[rxConsumed * 2], rxLen * sizeof(uint16_t) * 2);
            rxConsumed += rxLen;
            rxTimestamp += rxLen;
            if (rxResyncCandidate)
                rxResyncCandidate += rxLen;
            timestamp += rxLen;
            buf += (2 * rxLen);
            len -= rxLen;
            rlen += rxLen;
            samplesRead += rxLen;
            if (len <= 0)
                break;
        }
        if (++rxBufIndex >= rxBufCount) {
            // We are out of buffered data so we need to actually read some more
            rxBufCount = 0;
            rxBufIndex = 0;
            int bufCount = (len + PKT_SAMPLES_USE - 1) / PKT_SAMPLES_USE;
            if (bufCount > PKT_BUFFERS)
                bufCount = PKT_BUFFERS;
            int status = bladerf_sync_rx(bdev, &rxBuffer, PKT_SAMPLES_RAW * bufCount, NULL, DEFAULT_STREAM_TIMEOUT);
            if (status < 0) {
                LOG(ERR, "Samples RX failed at %llu: %s", rxTimestamp, bladerf_strerror(status) );
                break;
            }
            rxBufCount = bufCount;
            // Compute averages and peak values
            int iMin = 32767;
            int iMax = -32767;
            int iAvg = 0;
            int qMin = 32767;
            int qMax = -32767;
            int qAvg = 0;
            int wPeak = 0;
            TIMESTAMP t = rxTimestamp;
            TIMESTAMP peak = 0;
            for (int b = 0; b < bufCount; b++) {
                int16_t* s = rxBuffer[b].samples;
                for (int n = PKT_SAMPLES_USE; n > 0; n--, t++) {
                    int i = *s++;
                    int q = *s++;
                    iAvg += i;
                    qAvg += q;
                    if (!rxShowInfo)
                        continue;
                    if (iMin > i)
                        iMin = i;
                    if (iMax < i)
                        iMax = i;
                    if (qMin > q)
                        qMin = q;
                    if (qMax < q)
                        qMax = q;
                    if (wPeak < iMax) { wPeak = iMax; peak = t; }
                    if (wPeak < -iMin) { wPeak = -iMin; peak = t; }
                    if (wPeak < qMax) { wPeak = qMax; peak = t; }
                    if (wPeak < -qMin) { wPeak = -qMin; peak = t; }
                }
            }
            iAvg /= (bufCount * PKT_SAMPLES_USE);
            qAvg /= (bufCount * PKT_SAMPLES_USE);
            if (rxShowInfo) {
                if (rxShowInfo > 0)
                    rxShowInfo--;
            }
            // DC offsets compensation feedback using an exponential moving average
            mRxAverageI = ((RX_AVERAGE_DAMPING - 1) * mRxAverageI / RX_AVERAGE_DAMPING) + iAvg;
            mRxAverageQ = ((RX_AVERAGE_DAMPING - 1) * mRxAverageQ / RX_AVERAGE_DAMPING) + qAvg;
            int corrI = mRxCorrectionI;
            int corrQ = mRxCorrectionQ;
            if ((mRxAverageI > mRxMaxOffset) && (corrI < MAX_RX_DC_OFFSET)) {
                corrI++;
                mRxAverageI = 0;
            }
            else if ((mRxAverageI < -mRxMaxOffset) && (corrI > -MAX_RX_DC_OFFSET)) {
                corrI--;
                mRxAverageI = 0;
            }
            if ((mRxAverageQ > mRxMaxOffset) && (corrQ < MAX_RX_DC_OFFSET)) {
                corrQ++;
                mRxAverageQ = 0;
            }
            else if ((mRxAverageQ < -mRxMaxOffset) && (corrQ > -MAX_RX_DC_OFFSET)) {
                corrQ--;
                mRxAverageQ = 0;
            }
            if (mDcCorrect && ((corrI != mRxCorrectionI) || (corrQ != mRxCorrectionQ))) {
                LOG(INFO, "Adjusting Rx DC offsets: %d %d", corrI, corrQ );
                setRxOffsets(corrI, corrQ);
            }
        }
        rxConsumed = 0;
        TIMESTAMP tStamp = (((uint64_t)usrp_to_host_u32(rxBuffer[rxBufIndex].time_hi)) << 31) | (usrp_to_host_u32(rxBuffer[rxBufIndex].time_lo) >> 1);

        if (tStamp != rxTimestamp) {
            long long delta = (long long)(tStamp - rxTimestamp);
            if (abs(delta) < 1000 || (tStamp == rxResyncCandidate)) {
                LOG(NOTICE, "RX Timestamp adjusted by %lld to %llu.", delta, tStamp );
                rxTimestamp = tStamp;
                rxResyncCandidate = 0;
                if (overrun && (delta > 0))
                    *overrun = true;
            }
            else {
                LOG(WARNING, "RX Timestamp jumped by %lld at %llu in buffer %d / %d",
                  delta,
                  rxTimestamp,
                  rxBufIndex,
                  rxBufCount );

                rxResyncCandidate = tStamp;
            }
        }
        // Rob: disabled temporarily
        /*
        uint32_t flags = usrp_to_host_u32(rxBuffer[rxBufIndex].flags);
        if ((flags >> 28) & 0x04) {
            if (underrun) *underrun = true;
        }
        if (RSSI)
            *RSSI = (flags >> 21) & 0x3f;
        */
    }

    return rlen;
}

//#include <unistd.h>
//#include <fcntl.h>
//static int bf = -1;

int bladeRFDevice::writeSamples(short *buf, int len, bool *underrun,
        TIMESTAMP timestamp,
        bool isControl)
{
    if (len <= 0 || !bdev) return 0;

    if (txShowInfo) {
        if (txShowInfo > 0)
            txShowInfo--;
        int wMin = 0;
        int wMax = 0;
        for (int si = 0; si < 2*len; si++) {
            if (wMin > buf[si])
                wMin = buf[si];
            if (wMax < buf[si])
                wMax = buf[si];
        }
    }

//if (bf < 0)
//    bf = ::open("tx.raw",O_CREAT|O_TRUNC|O_WRONLY);
//if (bf >= 0)
//    ::write(bf,buf,4*len);

    writeLock.lock();
    int olen = len;

	// DAVID COMMENT:  The samples in buf[] are assumed to be in the
	// proper scaling.
	//
    // If there are gaps in timestamp drop the buffer and resynch
    TIMESTAMP tStamp = txTimestamp + txBuffered;
    if (timestamp != tStamp) {
        LOG(WARNING, "TX Timestamp difference: %llu vs %llu", timestamp, tStamp );
        txBuffered = 0;
        txTimestamp = timestamp;
    }

    int bufSpace = PKT_SAMPLES_USE - txBuffered;
    if (bufSpace > 0) {
        // There is space left in buffer - fill it
        if (bufSpace > len)
            bufSpace = len;
        if (pulseMode < 0)
            memcpy(&txBuffer.samples[txBuffered * 2], buf, bufSpace * sizeof(uint16_t) * 2);
        else if (pulseMode < 2)
            memset(&txBuffer.samples[txBuffered * 2], 0, bufSpace * sizeof(uint16_t) * 2);
        else {
            int16_t *p = txBuffer.samples + (txBuffered * 2);
            TIMESTAMP t = tStamp;
            for (int n = bufSpace; n > 0; n--, p+=2)
                switch ((t++) % pulseMode) {
                    case 0:
                        p[1] = p[0] = 1795;
                        break;
                    case 1:
                        p[1] = p[0] = -1795;
                        break;
                    default:
                        p[1] = p[0] = 0;
                }
        }
        txBuffered += bufSpace;
        buf += (2 * bufSpace);
        len -= bufSpace;
    }

    while (txBuffered >= PKT_SAMPLES_USE) {
        // We have a full buffer - send it
        // Each (I,Q) of a sample is counted individually by bladeRF
        txBuffer.time_lo = host_to_usrp_u32(txTimestamp << 1);
        txBuffer.time_hi = host_to_usrp_u32(txTimestamp >> 31);
        txBuffer.rsvd = 0xdeadbeef;
        txBuffer.flags = (uint32_t)-1;
        int status = bladerf_sync_tx(bdev, &txBuffer, PKT_SAMPLES_RAW, NULL, DEFAULT_STREAM_TIMEOUT);
        if (status < 0) {
            LOG(ERR, "Samples TX failed at %llu: %s", txTimestamp, bladerf_strerror(status) );
        }
        else
            samplesWritten += PKT_SAMPLES_USE;
        txTimestamp += PKT_SAMPLES_USE;
        txBuffered = len;
        if (!len)
            break;
        if (txBuffered > PKT_SAMPLES_USE)
            txBuffered = PKT_SAMPLES_USE;
        if (pulseMode < 0)
            memcpy(txBuffer.samples, buf, txBuffered * sizeof(uint16_t) * 2);
        else if (pulseMode < 2)
            memset(txBuffer.samples, 0, txBuffered * sizeof(uint16_t) * 2);
        else {
            int16_t *p = txBuffer.samples;
            TIMESTAMP t = txTimestamp;
            for (int n = txBuffered; n > 0; n--, p+=2)
                switch ((t++) % pulseMode) {
                    case 0:
                        p[1] = p[0] = 1795;
                        break;
                    case 1:
                        p[1] = p[0] = -1795;
                        break;
                    default:
                        p[1] = p[0] = 0;
                }
        }
        buf += (2 * txBuffered);
        len -= txBuffered;
    }
    assert(len == 0);

    writeLock.unlock();

    return olen;
}

bool bladeRFDevice::setVCTCXOrad1(unsigned int adj)
{
    return bladerf_dac_write(bdev, adj << 8) >= 0;
}

bool bladeRFDevice::setLoopback(bladerf_loopback loopback)
{
    writeLock.lock();
    int status = bladerf_set_loopback(bdev, loopback);
    writeLock.unlock();
    if (status < 0) {
        LOG(ERR, "Setting loopback mode failed: %s", bladerf_strerror(status) );
        return false;
    }
    return true;
}

bool bladeRFDevice::setVCTCXO(unsigned int wAdjFreq)
{
    LOG(INFO, "set VCTCXO: %u", wAdjFreq );

    ScopedLock myLock(writeLock);
    return setVCTCXOrad1(wAdjFreq);
}

bool bladeRFDevice::setRxOffsets(int corrI, int corrQ)
{
    if ((abs(corrI) > MAX_RX_DC_OFFSET) || (abs(corrQ) > MAX_RX_DC_OFFSET))
        return false;
    if ((corrI == mRxCorrectionI) && (corrQ == mRxCorrectionQ))
        return true;
    writeLock.lock();
    mRxCorrectionI = corrI;
    mRxCorrectionQ = corrQ;
    corrI <<= SHIFT_RX_DC;
    corrQ <<= SHIFT_RX_DC;
    int status = bladerf_set_correction(bdev, BLADERF_MODULE_RX, BLADERF_CORR_LMS_DCOFF_I, corrI);
    if (status >= 0)
        status = bladerf_set_correction(bdev, BLADERF_MODULE_RX, BLADERF_CORR_LMS_DCOFF_Q, corrQ);
    writeLock.unlock();
    if (status < 0) {
        LOG(ERR, "Setting RX DC correction failed: %s", bladerf_strerror(status) );
        return false;
    }
    return true;
}

bool bladeRFDevice::setTxOffsets(int corrI, int corrQ)
{
    if ((abs(corrI) > MAX_TX_DC_OFFSET) || (abs(corrQ) > MAX_TX_DC_OFFSET))
        return false;
    corrI <<= SHIFT_TX_DC;
    corrQ <<= SHIFT_TX_DC;
    writeLock.lock();
    int status = bladerf_set_correction(bdev, BLADERF_MODULE_TX, BLADERF_CORR_LMS_DCOFF_I, corrI);
    if (status >= 0)
        status = bladerf_set_correction(bdev, BLADERF_MODULE_TX, BLADERF_CORR_LMS_DCOFF_Q, corrQ);
    writeLock.unlock();
    if (status < 0) {
        LOG(ERR, "Setting TX DC correction failed: %s", bladerf_strerror(status) );
        return false;
    }
    return true;
}


bool bladeRFDevice::setTxFreq(double wFreq, double wAdjFreq)
{
    ScopedLock myLock(writeLock);
    return (bladerf_set_frequency(bdev, BLADERF_MODULE_TX, wFreq) >= 0)
        && setVCTCXOrad1((unsigned int)wAdjFreq);
}

bool bladeRFDevice::setRxFreq(double wFreq, double wAdjFreq)
{
    ScopedLock myLock(writeLock);
    return (bladerf_set_frequency(bdev, BLADERF_MODULE_RX, wFreq) >= 0)
        && setVCTCXOrad1((unsigned int)wAdjFreq);
}


// Factory calibration handling

unsigned int bladeRFDevice::getFactoryValue(const std::string &name)
{
    if (name == "freq") {
        uint16_t trim = 0x8000;
        bladerf_get_vctcxo_trim(bdev, &trim);
        trim = trim >> 8;
        return trim;
    }
    // TODO : need a better error condition here
    else
        return 0;
}

bool bladeRFDevice::runCustom(const std::string &command)
{
    if (command == "normal" || command == "on") {
        pulseMode = -1;
        setLoopback(BLADERF_LB_NONE);
    }
    else if (command == "unmodulated" || command == "off")
        pulseMode = 0;
    else if (command == "modulated")
        pulseMode = -1;
    else if (command.substr(0,6) == "pulsed") {
        if (::sscanf(command.c_str(),"%*s %d",&pulseMode) != 1 || pulseMode < 2)
            pulseMode = 2;
        else if (pulseMode > 1000000)
            pulseMode = 1000000;
    }
    else if (command == "loopback none")
        setLoopback(BLADERF_LB_NONE);
    else if (command == "loopback lna1")
        setLoopback(BLADERF_LB_RF_LNA1);
    else if (command == "loopback lna2")
        setLoopback(BLADERF_LB_RF_LNA2);
    else if (command == "loopback lna3")
        setLoopback(BLADERF_LB_RF_LNA3);
    else if (command.substr(0,10) == "tx offsets") {
        int corrI = 0;
        int corrQ = 0;
        if (::sscanf(command.c_str(),"%*s %*s %d %d",&corrI,&corrQ) != 2)
            return false;
        return setTxOffsets(corrI, corrQ);
    }
    else if (command == "rx offsets auto")
        mDcCorrect = true;
    else if (command.substr(0,10) == "rx offsets") {
        int corrI = 0;
        int corrQ = 0;
        if (::sscanf(command.c_str(),"%*s %*s %d %d",&corrI,&corrQ) != 2)
            return false;
        bool tmp = mDcCorrect;
        mDcCorrect = false;
        if (setRxOffsets(corrI, corrQ))
            return true;
        mDcCorrect = tmp;
        return false;
    }
    else if (command.substr(0,8) == "rx gain1") {
        int gain = -1;
        if (::sscanf(command.c_str(),"%*s %*s %d",&gain) != 1)
            return false;
        if (BLADERF_RXVGA1_GAIN_MIN > gain || gain > BLADERF_RXVGA1_GAIN_MAX)
            return false;
        writeLock.lock();
        int status = bladerf_set_rxvga1(bdev, gain);
        if (status >= 0)
            mRxGain1 = gain;
        else {
            LOG(ERR, "Error setting RX gain1: %s", bladerf_strerror(status) );
        }
        writeLock.unlock();
    }
    else if (command == "rx show on")
        rxShowInfo = -1;
    else if (command == "rx show off")
        rxShowInfo = 0;
    else if (command.substr(0,7) == "rx show") {
        unsigned int n = 0;
        if (::sscanf(command.c_str(),"%*s %*s %u",&n) != 1)
            return false;
        rxShowInfo = n;
    }
    else if (command == "tx show on")
        txShowInfo = -1;
    else if (command == "tx show off")
        txShowInfo = 0;
    else if (command.substr(0,7) == "tx show") {
        unsigned int n = 0;
        if (::sscanf(command.c_str(),"%*s %*s %u",&n) != 1)
            return false;
        txShowInfo = n;
    }
    else if (command == "spam on")
        spammy = true;
    else if (command == "spam off")
        spammy = false;
    else
        return false;
    return true;
}

// Device creation factory

RadioDevice *RadioDevice::make(int &sps, bool skipRx)
{
#ifdef MIN_OVERSAMPLING
    if (sps < MIN_OVERSAMPLING)
        sps = MIN_OVERSAMPLING;
#endif
    return new bladeRFDevice(sps, skipRx);
}

void RadioDevice::staticInit()
{
}
