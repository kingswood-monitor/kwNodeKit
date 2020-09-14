/**
 * kwLoraSM.ch: Environment Sensor node firmware
 * Copyright (c) 2020 Richard J. Lyon
 * 
 * See LICENSE for terms.
 */

#include "kwTransport.h"
#include "kwSensor.h"
#include "LoRa.h"

#define DEFAULT_POWER 17

class kwLoraSM : public kwTransport, public kwSensor
{
public:
    /**
     * Create a new instance of the kwLoraSM class.
     */
    kwLoraSM();

    /**
     * Create a new instance of the kwLoraSM class.
     * 
     * @param queue: The queue to publish incoming packets on.
     */
    kwLoraSM(QueueHandle_t xQueue);

    // kwTransport interface
    bool startTransport();
    bool sendPacket(uint16_t uiTimeStamp, uint8_t *packetBuffer, uint8_t bytesWritten);

    // kwSensor interface
    bool startSensor();
    bool readAndEncodeMeasurements(
        pb_ostream_t *ostream,
        const pb_field_iter_t *field,
        void *const *arg,
        bool rbeFlag);

    // kwLoraSensor methods
    void setPower(uint8_t txPower);
    int16_t rssi();
    int16_t snr();
    int16_t freqError();

    uint8_t parsePacket(uint8_t *buffer);

    // kwLoraSensor properties
    QueueHandle_t *xQueue; // Holds received messages for processing

private:
    uint8_t txPower_ = {DEFAULT_POWER}; // valid levels +2 to +20
    double frequency_ = {868E6};
    void LoRa_rxMode();
    void LoRa_txMode();
    void LoRa_sendMessage(uint8_t *buffer, size_t size);
    static void onTxDone();
};

void onReceive(int packetSize);
