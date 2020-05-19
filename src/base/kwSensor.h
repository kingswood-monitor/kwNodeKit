/**
 * kwSensor.h: Environment Sensor node firmware
 * Copyright (c) 2020 Richard J. Lyon
 * 
 * See LICENSE for terms.
 */
#pragma once
#include "packet.pb.h"

class kwSensor
{
public:
    struct Reading
    {
        double value;
        bool hasChanged;
    };

    struct RBEConfig
    {
        double delta;
        double sentinel;
        double last_reading;
    };

    // constructors //////////////////////////////////////////

    kwSensor();
    kwSensor(SensorName sensor);
    virtual ~kwSensor(){};

    // kwSensor interface ////////////////////////////////////

    virtual bool startSensor() = 0;         // initialise sensor
    virtual bool readAndEncodeMeasurements( // read sensor and encode as protobuf
        pb_ostream_t *ostream,
        const pb_field_iter_t *field,
        void *const *arg,
        bool rbeFlag) = 0;

    // methods ///////////////////////////////////////////////

    bool isInstalled();
    uint8_t name();

protected:
    bool isInstalled_ = false;
    SensorName name_ = SensorName_UnknownSensor;
    RBEConfig rbeTemperatureConfig = {0.1, -99, -99};
    RBEConfig rbeHumidityConfig = {1.0, 0, 0};
    RBEConfig rbeLightConfig = {1.0, 0, 0};
    RBEConfig rbeVoltageConfig = {0.01, 0, 0};
    RBEConfig rbeRSSIConfig = {1, -999, -999};
    RBEConfig rbeSNRConfig = {1, -999, -999};
    RBEConfig rbeFrequencyErrorConfig = {1, -999, -999};
};

bool processMeasurement(Measurement &measurement, double value, kwSensor::RBEConfig &rbeConfig, bool rbeFlag, int measurementTag, const char *label, pb_ostream_t *ostream, const pb_field_iter_t *field);
kwSensor::Reading makeReading(double &value, kwSensor::RBEConfig &config);
bool hasChanged(double value, kwSensor::RBEConfig &config);
bool encode_field(Measurement *m, pb_ostream_t *ostream, const pb_field_iter_t *field);
void printMeasurement(const char *label, kwSensor::Reading *reading);
