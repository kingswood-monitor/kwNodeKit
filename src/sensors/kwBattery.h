#pragma once
/**
 * kwBattery.h
 * Copyright (c) 2020 Richard J. Lyon
 * 
 * See LICENSE for terms.
 */

#include "kwSensor.h"

/**
 * Implements a virtual battery sensor. 
 */

class kwBattery : public kwSensor
{
public:
    /*-----------------------------------------------------------
     * CONSTRUCTORS
     *----------------------------------------------------------*/

    /**
     * @brief Constructor with battery sensing pin.
     * Sets the sensor name per protobuf SensorName type, and the battery voltage pin.
     * 
     * @param pin The battery voltage pin. 
     */
    kwBattery(uint8_t pin) : kwSensor(SensorName_BATTERY), pin_(pin){};

    /*-----------------------------------------------------------
     * kwSensor VIRTUAL INTERFACE METHODS
     *----------------------------------------------------------*/

    /**
     * @brief Start the sensor. 
     * The concrete implementation carries out any functions required to initialise
     * and start the sensor.
     * 
     * @return TRUE if the sensor started.
     */
    bool startSensor();

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
    bool readAndEncodeMeasurements(
        pb_ostream_t *ostream,
        const pb_field_iter_t *field,
        void *const *arg,
        bool rbeFlag);

private:
    /*-----------------------------------------------------------
     * PRIVATE PROPERTIES
     *----------------------------------------------------------*/

    //* The battery voltage pin */
    const uint8_t pin_ = 0;
};
