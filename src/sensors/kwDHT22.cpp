/**
 * kwDHT22.cpp: Environment Sensor node firmware
 * Copyright (c) 2020 Richard J. Lyon
 * 
 * See LICENSE for terms.
 */

#include <pb_encode.h>
#include "packet.pb.h"

#include "kwDHT22.h"

/*-----------------------------------------------------------
 * kwSensor VIRTUAL INTERFACE METHODS
 *----------------------------------------------------------*/

bool kwDHT22::startSensor()
{
    // FIXME Check the logic of how sensors are started and the flag is set

    bool isInstalled_ = false;

    /** Set the data pin */
    dht_.setup(pin_);

    /** The DHT22 returns 0 if the device is working */
    if (dht_.getStatus() == 0)
    {
        isInstalled_ = true;
    }

    return isInstalled_;
}

bool kwDHT22::readAndEncodeMeasurements(
    pb_ostream_t *ostream,
    const pb_field_iter_t *field,
    void *const *arg,
    bool rbeFlag)
{
    /** Flag to capture that encoding was successful */
    bool bSuccess = false;

    /** Define a measurement with default values and set the sensor name. */
    Measurement measurement = Measurement_init_default;
    measurement.sensor = name_;

    /** Get the temperature */
    double current_temperature = dht_.getTemperature();

    /** Encode it and capture the success*/
    if (dht_.getStatus() == 0)
    {
        bSuccess |= processMeasurement(
            measurement,
            current_temperature,
            rbeTemperatureConfig,
            rbeFlag,
            Measurement_temperature_tag,
            "T",
            ostream,
            field);
    }

    /** Get the humidity */
    double current_humidity = dht_.getHumidity();

    /** Encode it and capture the success*/
    if (dht_.getStatus() == 0)
    {
        bSuccess |= processMeasurement(
            measurement,
            current_humidity,
            rbeHumidityConfig,
            rbeFlag,
            Measurement_humidity_tag,
            "H",
            ostream,
            field);
    }

    return bSuccess;
}
