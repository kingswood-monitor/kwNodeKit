/**
 * kwDHT22.cpp: Environment Sensor node firmware
 * Copyright (c) 2020 Richard J. Lyon
 * 
 * See LICENSE for terms.
 */

#include <pb_encode.h>
#include "packet.pb.h"

#include "kwDHT22.h"

// kwSensor interface ////////////////////////////////////

bool kwDHT22::
    startSensor()
{
    dht_.setup(pin_);
    if (dht_.getStatus() == 0)
    {
        isInstalled_ = true;
    }
    return isInstalled_;
}

bool kwDHT22::
    readAndEncodeMeasurements(pb_ostream_t *ostream, const pb_field_iter_t *field, void *const *arg, bool rbeFlag)
{
    bool result = false;
    Measurement measurement = Measurement_init_default;
    measurement.sensor = name_;

    // temperature
    double current_temperature = dht_.getTemperature();
    if (dht_.getStatus() == 0)
    {
        result |= processMeasurement(measurement, current_temperature, rbeTemperatureConfig, rbeFlag, Measurement_temperature_tag, "T", ostream, field);
    }

    // humidity
    double current_humidity = dht_.getHumidity();
    if (dht_.getStatus() == 0)
    {
        result |= processMeasurement(measurement, current_humidity, rbeHumidityConfig, rbeFlag, Measurement_humidity_tag, "H", ostream, field);
    }

    return result;
}
