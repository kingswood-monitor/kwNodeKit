/**
 * kwBattery.cpp
 * Copyright (c) 2020 Richard J. Lyon
 *
 * See LICENSE for terms.
 */

#include "packet.pb.h"
#include <Arduino.h>
#include <pb_encode.h>

#include "kwBattery.h"

/*-----------------------------------------------------------
 * kwSensor VIRTUAL INTERFACE METHODS
 *----------------------------------------------------------*/

bool kwBattery::startSensor()
{
    /** There is no convenient way of detecting if the device is battery
     * operated. So if it's been added, assume it's installed.
     */
    isInstalled(true);
    return true;
}

bool kwBattery::readAndEncodeMeasurements(pb_ostream_t *ostream, const pb_field_iter_t *field, void *const *arg,
                                          bool rbeFlag)
{
    /** Flag to capture that encoding was successful */
    bool bSuccess = false;

    /** Define a measurement with default values and set the sensor name. */
    Measurement measurement = Measurement_init_default;
    measurement.sensor = name_;

    /** Get the voltage */
    float measuredVbat = analogRead(pin_);
    measuredVbat *= 2;    // we divided by 2, so multiply back
    measuredVbat *= 3.3;  // Multiply by 3.3V, our reference voltage
    measuredVbat /= 1024; // convert to voltage

    /** Encode it and capture the success*/
    bSuccess |= processMeasurement(measurement, measuredVbat, rbeVoltageConfig, rbeFlag, Measurement_voltage_tag, "V",
                                   ostream, field);

    return bSuccess;
}