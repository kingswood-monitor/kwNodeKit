/**
 * kwHDC1080.cpp
 * Copyright (c) 2020 Richard J. Lyon
 *
 * See LICENSE for terms.
 */

#include "packet.pb.h"
#include <pb_encode.h>

#include "kwHDC1080.h"

/*-----------------------------------------------------------
 * kwSensor VIRTUAL INTERFACE METHODS
 *----------------------------------------------------------*/

bool kwHDC1080::startSensor()
{
    bool bDidStart = false;

    /** Start the device */
    hdc1080_.begin(0x40);

    /** If we can read the device id, it must have started */
    if ((hdc1080_.readDeviceId() == 4176))
    {
        bDidStart = true;
    }

    isInstalled(bDidStart);

    return bDidStart;
}

bool kwHDC1080::readAndEncodeMeasurements(pb_ostream_t *ostream, const pb_field_iter_t *field, void *const *arg,
                                          bool rbeFlag)
{
    /** Flag to capture that encoding was successful */
    bool bSuccess = false;

    /** Define a measurement with default values and set the sensor name. */
    Measurement measurement = Measurement_init_default;
    measurement.sensor = name_;

    //** Get the temperature */
    float current_temperature = hdc1080_.readTemperature();

    /** Encode it and capture the success*/
    bSuccess |= processMeasurement(measurement, current_temperature, rbeTemperatureConfig, rbeFlag,
                                   Measurement_temperature_tag, "T", ostream, field);

    //** Get the humidity */
    float current_humidity = hdc1080_.readHumidity();

    /** Encode it and capture the success*/
    bSuccess |= processMeasurement(measurement, current_humidity, rbeHumidityConfig, rbeFlag, Measurement_humidity_tag,
                                   "H", ostream, field);

    return bSuccess;
}