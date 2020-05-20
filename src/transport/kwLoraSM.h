/**
 * kwLoraSM.ch: Environment Sensor node firmware
 * Copyright (c) 2020 Richard J. Lyon
 * 
 * See LICENSE for terms.
 */

#include "kwTransport.h"
#include "kwSensor.h"

class kwLoraSM : public kwTransport, public kwSensor
{
public:
    // constructors
    kwLoraSM();
    kwLoraSM(uint8_t txPower);

    // kwTransport interface
    bool startTransport();
    bool sendPacket(uint8_t *packetBuffer, uint8_t bytesWritten);

    // kwSensor interface
    bool startSensor();
    bool readAndEncodeMeasurements(
        pb_ostream_t *ostream,
        const pb_field_iter_t *field,
        void *const *arg,
        bool rbeFlag);

    // kwLoraSensor methods
    int16_t rssi();
    int16_t snr();
    int16_t freqError();

private:
    uint8_t txPower_ = {17}; // valid levels +2 to +20
    double frequency_ = {868E6};
};

void onReceive(int packetSize);
void onTxDone();
void LoRa_rxMode();
void LoRa_txMode();
void LoRa_sendMessage(uint8_t *buffer, size_t size);