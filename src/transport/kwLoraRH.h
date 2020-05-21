/**
 * kwLora.h: Environment Sensor node firmware
 * Copyright (c) 2020 Richard J. Lyon
 * 
 * See LICENSE for terms.
 */

#include "kwTransport.h"
#include "kwSensor.h"

class kwLoraRH : public kwTransport, public kwSensor
{
public:
    // constructors
    kwLoraRH();
    kwLoraRH(uint8_t clientAddress, uint8_t txPower);

    // kwTransport interface
    bool startTransport();
    bool sendPacket(uint16_t packetID, uint8_t *packetBuffer, uint8_t bytesWritten);

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
    uint8_t serverAddress_ = {0};
    uint8_t txPower_ = {23}; // valid levels +5 to +23
    double frequency_ = {868.0};
};

// forward declarations

void printPacket(const char *prompt, uint16_t packetID, uint8_t *packetBuffer, uint8_t bytesWritten);
