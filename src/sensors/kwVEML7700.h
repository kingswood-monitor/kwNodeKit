#pragma once
/**
 * kwVEML7700.h: Environment Sensor node firmware
 * Copyright (c) 2020 Richard J. Lyon
 * 
 * See LICENSE for terms.
 */

#include <VEML7700.h>
#include "kwSensor.h"

class kwVEML7700 : public kwSensor
{
public:
    // constructors
    kwVEML7700() : kwSensor(SensorName_VEML7700){};

    // kwSensor interface
    bool startSensor();
    bool readAndEncodeMeasurements(
        pb_ostream_t *ostream,
        const pb_field_iter_t *field,
        void *const *arg,
        bool rbeFlag);

private:
    VEML7700 veml_;
};