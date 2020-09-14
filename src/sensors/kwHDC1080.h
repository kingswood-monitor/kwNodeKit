#pragma once
/**
 * kwHDC1080.h:
 * Copyright (c) 2020 Richard J. Lyon
 * 
 * See LICENSE for terms.
 */

#include <Wire.h>
#include <ClosedCube_HDC1080.h>
#include "kwSensor.h"

/**
 * Implements an HDC1080 Sensor - a high accuracy temperature and humidity sensor
 * See: https://www.tindie.com/products/closedcube/hdc1080-high-accuracy-humiditytemperature-sensor/
 */
class kwHDC1080 : public kwSensor
{
public:
    /*-----------------------------------------------------------
     * CONSTRUCTORS
     *----------------------------------------------------------*/

    /**
     * @brief Default constructor.
     * Sets the sensor name per protobuf SensorName type.
     */
    kwHDC1080() : kwSensor(SensorName_HDC1080){};

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

    /*-----------------------------------------------------------
     * PRIVATE PROPERTIES
     *----------------------------------------------------------*/
private:
    /** The sensor object */
    ClosedCube_HDC1080 hdc1080_;
};