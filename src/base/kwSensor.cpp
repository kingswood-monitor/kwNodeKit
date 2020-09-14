#include "debug.h"

#include <Arduino.h>
#include "kwSensor.h"
#include <pb_encode.h>

/*-----------------------------------------------------------
 * CONSTRUCTORS
 *----------------------------------------------------------*/

kwSensor::kwSensor() {}

kwSensor::kwSensor(SensorName name) : name_(name) {}

/*-----------------------------------------------------------
 * PUBLIC METHODS
 *----------------------------------------------------------*/

bool kwSensor::isInstalled() { return isInstalled_; }

SensorName kwSensor::name() { return name_; }

/*-----------------------------------------------------------
 * HELPERS
 *----------------------------------------------------------*/

bool processMeasurement(
    Measurement &measurement,
    double value,
    kwSensor::RBEConfig &rbeConfig,
    bool rbeFlag,
    int measurementTag,
    const char *label,
    pb_ostream_t *ostream,
    const pb_field_iter_t *field)
{
    kwSensor::Reading reading = makeReading(value, rbeConfig);
    if (!rbeFlag || (rbeFlag && reading.hasChanged))
    {
        measurement.which_type = measurementTag;
        measurement.type.temperature = reading.value;

        if (!encode_field(&measurement, ostream, field))
        {
            Serial.println(F("ERROR: Couldn't encode field"));
            return false;
        }
    }
    printMeasurement(label, &reading);
    return true;
}

kwSensor::Reading makeReading(
    double &value,
    kwSensor::RBEConfig &config)
{
    kwSensor::Reading m;
    m.value = value;
    m.hasChanged = hasChanged(value, config);

    return m;
}

bool hasChanged(
    double value,
    kwSensor::RBEConfig &config)
{
    if (abs(value - config.last_reading) >= config.delta)
    {
        config.last_reading = value;
        return true;
    }
    return false;
}

bool encode_field(
    Measurement *m,
    pb_ostream_t *ostream,
    const pb_field_iter_t *field)
{
    return pb_encode_tag_for_field(ostream, field) &&
           pb_encode_submessage(ostream, Measurement_fields, m);
}

void printMeasurement(const char *label, kwSensor::Reading *reading)
{
    if (reading->hasChanged)
    {
        Serial.print("*");
    }

    Serial.print(label);
    Serial.print(":");
    Serial.print(reading->value);
    Serial.print(" | ");
}
