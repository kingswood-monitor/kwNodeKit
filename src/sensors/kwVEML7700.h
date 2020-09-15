#pragma once
/**
 * kwVEML7700.h
 * Copyright (c) 2020 Richard J. Lyon
 *
 * See LICENSE for terms.
 */

#include "kwSensor.h"
#include <VEML7700.h>

/**
 * Implements a VEML7700 ambient light sensor
 * See: https://learn.adafruit.com/adafruit-veml7700/arduino
 */
class kwVEML7700 : public NodeInput
{
  public:
    /*-----------------------------------------------------------
     * CONSTRUCTORS
     *----------------------------------------------------------*/

    /**
     * @brief Default constructor.
     * Sets the sensor name per protobuf SensorName type.
     */
    kwVEML7700(SensorName name, uint8_t interval, u8 inter_humid u8_inter temp) : NodeInput()
    {
        this.name = name;
        this.interval = interval;
    };

    /**
     * @brief Default constructor.
     * Sets the sensor name per protobuf SensorName type.
     */
    kwVEML7700(SensorName name) : NodeInput()
    {
        kwVEML7700(name, 1, 1, 1);
    };

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
    bool start()
    {
        task = vTaskCreate(, , , , *this);
    }

    bool running()
    {
        return task == null;
    }

    SensorName name()
    {
        return this.name;
    }

    uint8_t interval()
    {
        return this.iterval;
    }

  private:
    /*-----------------------------------------------------------
     * PRIVATE PROPERTIES
     *----------------------------------------------------------*/

    //* The sensor object */
    VEML7700 veml_;
    SensorName name;
    TaskHandle_t task;
    uint8_t interval;
};

void vemlTask(void *veml)
{
    kwVEML7700 *veml = (kwVEML7700 *)veml;

    veml
}