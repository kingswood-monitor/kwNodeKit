/**
 * kwVEML7700.cpp
 * Copyright (c) 2020 Richard J. Lyon
 * 
 * See LICENSE for terms.
 */

#include <pb_encode.h>
#include "packet.pb.h"

#include "kwVEML7700.h"

/*-----------------------------------------------------------
 * kwSensor VIRTUAL INTERFACE METHODS
 *----------------------------------------------------------*/

bool kwVEML7700::startSensor()
{
    bool isInstalled_ = false;

    /** A temporary variable to allow us to check if the device is active */
    float lux;

    /** Start the device */
    veml_.begin();

    if (!veml_.getALSLux(lux))
    {
        isInstalled_ = true;
        /**
         * Coonfigure the device.
         * The default integration time of 100ms results in reading clipped to 2126.52
         * 50ms / Gain 1/8 seems to be 60klux max, for direct sunlight of 50k 
         */
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
    /** Flag to capture that encoding was successful */
    bool bSuccess = false;

    /** Variables to hold raw and corrected light readings */
    float raw, current_lux;

    /** Define a measurement with default values and set the sensor name. */
    Measurement measurement = Measurement_init_default;
    measurement.sensor = name_;

    /** Get the ambient light reading */
    veml_.getALSLux(raw);

    /** Correct for 50ms / Gain 1/8 (see datasheet) */
    current_lux = raw * 0.9216;

    /** Encode it and capture the success*/
    bSuccess |= processMeasurement(
        measurement,
        current_lux,
        rbeLightConfig,
        rbeFlag,
        Measurement_light_tag,
        "L",
        ostream,
        field);

    return bSuccess;
}