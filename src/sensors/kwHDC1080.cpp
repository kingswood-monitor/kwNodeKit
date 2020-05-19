/**
 * kwHDC1080.cpp: Environment Sensor node firmware
 * Copyright (c) 2020 Richard J. Lyon
 * 
 * See LICENSE for terms.
 */

#include <pb_encode.h>
#include "packet.pb.h"

#include "kwHDC1080.h"

// kwSensor interface

bool kwHDC1080::
    startSensor()
{
    hdc1080_.begin(0x40);
    if ((hdc1080_.readDeviceId() == 4176))
    {
        isInstalled_ = true;
    }
    return isInstalled_;
}

bool kwHDC1080::
    readAndEncodeMeasurements(pb_ostream_t *ostream, const pb_field_iter_t *field, void *const *arg, bool rbeFlag)
{
    bool result = false;

    Measurement measurement = Measurement_init_default;
    measurement.sensor = name_;

    // temperature
    float current_temperature = hdc1080_.readTemperature();
    result |= processMeasurement(measurement, current_temperature, rbeTemperatureConfig, rbeFlag, Measurement_temperature_tag, "T", ostream, field);

    // humidity
    float current_humidity = hdc1080_.readHumidity();
    result |= processMeasurement(measurement, current_humidity, rbeHumidityConfig, rbeFlag, Measurement_humidity_tag, "H", ostream, field);

    return result;
}