/**
 * kwBattery.cpp: Environment Sensor node firmware
 * Copyright (c) 2020 Richard J. Lyon
 * 
 * See LICENSE for terms.
 */
#include <Arduino.h>
#include <pb_encode.h>
#include "packet.pb.h"

#include "kwBattery.h"

// There is no convenient way of detecting if the device is battery operated.
// So if it's been added, assume it's installed.
bool kwBattery::
    startSensor()
{
    isInstalled_ = true;
    return isInstalled_;
}

bool kwBattery::
    readAndEncodeMeasurements(pb_ostream_t *ostream, const pb_field_iter_t *field, void *const *arg, bool rbeFlag)
{
    bool result = false;
    Measurement measurement = Measurement_init_default;
    measurement.sensor = name_;

    // voltage
    float measuredVbat = analogRead(pin_);
    measuredVbat *= 2;    // we divided by 2, so multiply back
    measuredVbat *= 3.3;  // Multiply by 3.3V, our reference voltage
    measuredVbat /= 1024; // convert to voltage

    result |= processMeasurement(measurement, measuredVbat, rbeVoltageConfig, rbeFlag, Measurement_voltage_tag, "V", ostream, field);

    return true;
}