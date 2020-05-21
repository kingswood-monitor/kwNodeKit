#pragma once
/**
 * kwTransport.ch: Environment Sensor node firmware
 * Copyright (c) 2020 Richard J. Lyon
 * 
 * See LICENSE for terms.
 */

#include <Arduino.h>
#include "kwLED.h"
#include "packet.pb.h"

/**
 * Base class for node radios.
 */

class kwTransport
{
public:
    kwTransport();
    virtual bool startTransport() = 0;                                        // start the radio
    virtual bool sendPacket(uint8_t *packetBuffer, uint8_t bytesWritten) = 0; // send a packet
    void isGateway(bool flag);
    bool isGateway();

protected:
    kwLED transportLed;

private:
    bool _isGateway = false;
};