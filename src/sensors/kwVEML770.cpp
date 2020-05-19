/**
 * kwVEML7700.cpp: Environment Sensor node firmware
 * Copyright (c) 2020 Richard J. Lyon
 * 
 * See LICENSE for terms.
 */

#include <pb_encode.h>
#include "packet.pb.h"

#include "kwVEML7700.h"

// kwSensor interface

bool kwVEML7700::
    startSensor()
{
    // default integration time of 100ms results in reading clipped to 2126.52
    // 50ms / Gain 1/8 seems to be 60klux max, for direct sunlight of 50k
    float lux;
    veml_.begin();

    if (!veml_.getALSLux(lux))
    {
        isInstalled_ = true;
        veml_.setIntegrationTime(VEML7700::als_itime_t::ALS_INTEGRATION_50ms);
        veml_.setGain(VEML7700::als_gain_t::ALS_GAIN_d8);
    }
    return isInstalled_;
}

bool kwVEML7700::
    readAndEncodeMeasurements(
        pb_ostream_t *ostream,
        const pb_field_iter_t *field,
        void *const *arg,
        bool rbeFlag)
{
    bool result = false;
    Measurement measurement = Measurement_init_default;
    measurement.sensor = name_;

    // light
    float raw, current_lux;
    veml_.getALSLux(raw);
    current_lux = raw * 0.9216; // 50ms / Gain 1/8

    result |= processMeasurement(
        measurement,
        current_lux,
        rbeLightConfig,
        rbeFlag,
        Measurement_light_tag,
        "L",
        ostream,
        field);

    return result;
}