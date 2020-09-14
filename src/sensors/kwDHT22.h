#pragma once
/**
 * kwDHT22.h
 * Copyright (c) 2020 Richard J. Lyon
 *
 * See LICENSE for terms.
 */

#include "DHT.h"
#include "kwSensor.h"

/**
 * Implements a DHT22 Sensor - a basic, low cost temperature and humidity sensor
 * See: https://www.adafruit.com/product/385
 */
class kwDHT22 : public kwSensor
{
  public:
    /*-----------------------------------------------------------
     * CONSTRUCTORS
     *----------------------------------------------------------*/

    /**
     * @brief Default constructor.
     * Sets the sensor name per protobuf SensorName type.
     */
    kwDHT22() : kwSensor(SensorName_DHT22){};

    /**
     * @brief Constructor with data pin.
     * Sets the sensor name per protobuf SensorName type, and dta pin.
     *
     * Example usage:
     * @code{c}
     * #include <kwDHT22.h>
     * const uint_8t DH22_DATA_PIN=21;
     * kwDHT22 *pDHT22 = new kwDHT22(DH22_DATA_PIN);
     * @endcode
     *
     * @param pin Data pin number.
     */
    kwDHT22(uint8_t pin) : kwSensor(SensorName_DHT22), pin_(pin){};

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
    bool readAndEncodeMeasurements(pb_ostream_t *ostream, const pb_field_iter_t *field, void *const *arg, bool rbeFlag);

  private:
    /*-----------------------------------------------------------
     * PRIVATE PROPERTIES
     *----------------------------------------------------------*/

    //* The data pin */
    uint8_t pin_ = 0;

    //* The sensor object */
    DHT dht_;
};