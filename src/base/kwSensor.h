#pragma once
/**
 * kwSensor.h
 * Copyright (c) 2020 Richard J. Lyon
 * 
 * See LICENSE for terms.
 */

#include "packet.pb.h"

/**
 * Defines a Sensor - a physical or virtual device that privodes one or more measurements
 * of a property e.g. temperature (physical) or virtual (LoRa RSSI).
 */
class kwSensor
{
public:
    /*-----------------------------------------------------------
     * STRUCTURES
     *----------------------------------------------------------*/

    /**
     * Used to record if a reading has changed. Required for Report By Exception processing.
     */
    struct Reading
    {
        double value;
        bool hasChanged;
    };

    /**
     * Defines configuration data for Report By Exception processing.
     */
    struct RBEConfig
    {
        /** Minimum threshold change - ignore changes less than this amount. */
        double delta;
        /** 
         * A value to use to represent "no previous value". For example, we might use
         * -99 for a temperature measurement, assuming that this will never be a valid measurement.
         */
        double sentinel;
        /** The last recorded reading to compute the current delta from. */
        double last_reading;
    };

    /*-----------------------------------------------------------
     * CONSTRUCTORS
     *----------------------------------------------------------*/

    /** Default constructor. */
    kwSensor();

    /**
     * @brief Construct a new Sensor with the supplied name.
     * 
     * A sensor has a name defined in the protobuf SensorName 
     * e.g. 'SensorName_DHT22'.
     * 
     * @param name The name of the sensor.
     */
    kwSensor(SensorName sensor);

    /*-----------------------------------------------------------
     * VIRTUAL INTERFACE METHODS
     *----------------------------------------------------------*/

    virtual ~kwSensor(){};

    /**
     * @brief Start the sensor. 
     * The concrete implementation carries out any functions required to initialise
     * and start the sensor.
     * 
     * @return TRUE if the sensor started.
     */
    virtual bool startSensor() = 0;

    /**
     * @brief Read and encode the sensor.
     * This function is provided by the protobuf library for encoding the sensor's information.
     * It's how a sensor describes what it is e.g. 'Temperature', and the value. Each concrete 
     * implementation provides the code required to read the sensor, and adds the metadata.
     * 
     * @param pb_ostream_t The stream to encode readings to (see nanopb).
     * @param field The field to encode (see nanopb).
     * @param arg Arguments to the encoding process (see nanopb).
     * @param rbeFlag Set TRUE to specify Report By Exception processing.
     * 
     * @return TRUE if the measurement encoded correctly.
     */
    virtual bool readAndEncodeMeasurements(
        pb_ostream_t *ostream,
        const pb_field_iter_t *field,
        void *const *arg,
        bool rbeFlag) = 0;

    /*-----------------------------------------------------------
     * PUBLIC METHODS
     *----------------------------------------------------------*/

    /**
     * isInstalled property getter.
     * 
     * @return TRUE if the sensor is installed.
     */
    bool isInstalled();

    /**
     * name property getter.
     * 
     * @return The name of the sensor.
     */
    SensorName name();

protected:
    /*-----------------------------------------------------------
     * PROTECTED PROPERTIES
     *----------------------------------------------------------*/

    /**
     * Set TRUE if this sensor is installed.
     */
    bool isInstalled_ = false;

    /**
     * The name of the sensor as defined in protobuf
     */
    SensorName name_ = SensorName_UnknownSensor;

    /**
     * Default Report By Exception configuration settings for
     * the measurements defined in the protobuf protocol.
     */
    RBEConfig rbeTemperatureConfig = {0.1, -99, -99};
    RBEConfig rbeHumidityConfig = {1.0, 0, 0};
    RBEConfig rbeLightConfig = {1.0, 0, 0};
    RBEConfig rbeVoltageConfig = {0.01, 0, 0};
    RBEConfig rbeRSSIConfig = {1, -999, -999};
    RBEConfig rbeSNRConfig = {1, -999, -999};
    RBEConfig rbeFrequencyErrorConfig = {1, -999, -999};
};

/*-----------------------------------------------------------
 * HELPERS
 *----------------------------------------------------------*/

/**
 * @brief Set the measurement's type and value, and encode it.
 * If Report By Exception is enabled, and the change in reading is less
 * than the configured threshold, ignore it.
 * 
 * @param measurement The measurement to process.
 * @param value The value of the measurement.
 * @param rbeConfig The configuration data for Report By Exception processing.
 * @param rbeFlag Flag to set Roper By Exception processing.
 * @param measurementTag The type of measurement as defined in the protobuf protocol e.g. "temperature".
 * @param label A label that is used to indicate the measurement in debug output.
 * @param ostream The stream to encode the result to (see nanopb).
 * @param field The field to encode (see nanopb).
 * 
 * @return TRUE if the reading has changed and was encoded successfully.
 */
bool processMeasurement(
    Measurement &measurement,
    double value,
    kwSensor::RBEConfig &rbeConfig,
    bool rbeFlag,
    int measurementTag, // FIXME this should be type Measurement
    const char *label,
    pb_ostream_t *ostream,
    const pb_field_iter_t *field);

/**
 * @brief Build a Reading structure to manage Report By Exception change detection
 * 
 * @param value The value to evaluate for changes.
 * @param config The Report By Exception configuration data specifying the change threshold.
 * 
 * @return The Reading structure
 */
kwSensor::Reading makeReading(
    double &value,
    kwSensor::RBEConfig &config);

/** 
 * @brief Test if the reading has changed based on current and previous values and a threshold.
 * 
 * @param value The value to evaluate for changes.
 * @param config The Report By Exception configuration data specifying the change threshold.
 * 
 * @return TRUE if the change in value exceeds the threshold.
 */
bool hasChanged(
    double value,
    kwSensor::RBEConfig &config);

/**
 * @brief The nanopb library function for encoding the field
 * 
 * @param m The measurement to encode.
 * @param ostream The stream to encode the result to (see nanopb).
 * @param field The field to encode (see nanopb).
 * 
 * @return TRUE if encoding succeeded.
 */
bool encode_field(
    Measurement *m,
    pb_ostream_t *ostream,
    const pb_field_iter_t *field);

/**
 * @brief Print the measurement to the terminal in a useful format.
 * Values are labelled, with changed values marked with a '*'.
 * 
 * @param label The label to prefix the reading with e.g. "T"
 * @param reading The reading to print
 */
void printMeasurement(
    const char *label,
    kwSensor::Reading *reading);
