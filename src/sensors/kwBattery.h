#pragma once
/**
 * kwBattery.h: Environment Sensor node firmware
 * Copyright (c) 2020 Richard J. Lyon
 * 
 * See LICENSE for terms.
 */
#include "kwSensor.h"

/**
 * Represents the battery on an Adafruit 32u4 device, returning the voltage. The class
 * is constructed with the pin that the battery voltage is present on.
 */

class kwBattery : public kwSensor
{
public:
    // Constructors
    kwBattery(uint8_t pin) : kwSensor(SensorName_BATTERY), pin_(pin){};

    bool startSensor();
    bool readAndEncodeMeasurements(pb_ostream_t *ostream, const pb_field_iter_t *field, void *const *arg, bool rbeFlag);
    uint8_t pin();

private:
    const uint8_t pin_ = 0;
};
