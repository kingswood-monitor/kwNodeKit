/**
 * kwSensor.h
 * Copyright (c) 2020 Richard J. Lyon
 *2
 * See LICENSE for terms.
 */

#pragma once

#include "packet.pb.h"

/**
 * Defines an input to the system - a physical or virtual device that privodes one
 * or more measurements of a property e.g. temperature (physical) or virtual (LoRa RSSI).
 */
class NodeInput
{
  public:
    NodeInput()
    {
    }
    virtual ~NodeInput()
    {
    }

    /**
     * @brief Start the input.
     *
     * The concrete implementation carries out any functions required to initialise
     * and start the sensor. It is expected that the implementation start a FreeRTOS
     * task to push measurements onto the queue.
     *
     * @param queue The queue to push inputs onto.
     *
     * @return TRUE if the sensor started.
     */
    virtual bool start(QueueHandle_t queue) = 0;

    /**
     * name property getter.
     *
     * @return The name of the sensor.
     */
    virtual SensorName name() = 0;
};
