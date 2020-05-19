#pragma once
/**
 * kwHDC1080.h: Environment Sensor node firmware
 * Copyright (c) 2020 Richard J. Lyon
 * 
 * See LICENSE for terms.
 */

#include <Wire.h>
#include <ClosedCube_HDC1080.h>
#include "kwSensor.h"

class kwHDC1080 : public kwSensor
{
public:
    // Constructor
    kwHDC1080() : kwSensor(SensorName_HDC1080){};

    // kwSensor interface
    bool startSensor();
    bool readAndEncodeMeasurements(pb_ostream_t *ostream, const pb_field_iter_t *field, void *const *arg, bool rbeFlag);

private:
    ClosedCube_HDC1080 hdc1080_;
};