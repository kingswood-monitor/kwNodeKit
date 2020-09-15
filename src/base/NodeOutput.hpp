#pragma once
/**
 * kwTransport.h
 * Copyright (c) 2020 Richard J. Lyon
 *
 * See LICENSE for terms.
 */

#pragma once

#include "packet.pb.h"
#include <Arduino.h>

/**
 * Defines an output from the system - a radio device (e.g. WiFi or LoRa) for transmitting data over.
 */
class NodeOutput
{
  public:
    kwTransport()
    {
    }
    virtual ~kwTransport()
    {
    }

    /**
     * @brief Initialise the radio and configure it for transmission.
     *
     * It is expected that the implementation start a FreeRTOS
     * task to push measurements onto the queue.
     */
    virtual bool start(QueueHandle_t queue) = 0;
};