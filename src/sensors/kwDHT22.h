#pragma once
/**
 * kwDHT22.h: Environment Sensor node firmware
 * Copyright (c) 2020 Richard J. Lyon
 * 
 * See LICENSE for terms.
 */

#include "DHT.h"
#include "kwSensor.h"

class kwDHT22 : public kwSensor
{
public:
    // Constructors
    kwDHT22() : kwSensor(SensorName_DHT22){};
    kwDHT22(uint8_t pin) : kwSensor(SensorName_DHT22), pin_(pin){};

    // kwSensor interface
    bool startSensor();
    bool readAndEncodeMeasurements(pb_ostream_t *ostream, const pb_field_iter_t *field, void *const *arg, bool rbeFlag);

private:
    uint8_t pin_ = 0;
    DHT dht_;
};

bool encode_field(Measurement *m, pb_ostream_t *ostream, const pb_field_iter_t *field);
void printMeasurement(const char *label, kwSensor::Reading *reading);