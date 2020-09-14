#pragma once
/**
 * kwTransport.h
 * Copyright (c) 2020 Richard J. Lyon
 * 
 * See LICENSE for terms.
 */

#include <Arduino.h>
#include "packet.pb.h"

/**
 * Defines a Transport - a radio device (e.g. WiFi or LoRa) for transmitting data over. 
 * Specifies Absract Base Class methods for starting the transport and sending data. It 
 * can be configured to invert LoRa I/Q signals to provide gateway functionality. 
 * 
 * FIXME gateway functionality needs to be pushed into LoRa classes as it doesn't 
 * make sense in a WiFi link.
 */
class kwTransport
{
public:
    /*-----------------------------------------------------------
    * CONSTRUCTORS
    *----------------------------------------------------------*/

    /** Default constructor. */
    kwTransport();

    /*-----------------------------------------------------------
    * VIRTUAL INTERFACE METHODS
    *----------------------------------------------------------*/

    /**
     * Initialise the radio and configure it for transmission.
     */
    virtual bool startTransport() = 0;

    /** 
     * Send a packet of information over the channel.
     * 
     * @param uiTimeStamp A number allowing the packet to be identified.
     * @param packetBuffer A pointer to a buffer containing the information to be transmitted.
     * @param bytesWritten The number of bytes to transmit.
     */
    virtual bool sendPacket(
        uint16_t uiTimeStamp,
        uint8_t *packetBuffer,
        uint8_t bytesWritten) = 0;

    /*-----------------------------------------------------------
     * PUBLIC METHODS
     *----------------------------------------------------------*/

    /**
     * isGateway property setter.
     * 
     * @param flag Boolean TRUE if this is a gateway
     */
    void isGateway(bool flag);

    /**
     * isGateway property getter.
     * 
     * @return TRUE of the transport defines a gateway.
     */
    bool isGateway();

private:
    /*-----------------------------------------------------------
     * PRIVATE PROPERTIES
     *----------------------------------------------------------*/

    /**
     * Set TRUE if this transport is a gateway.
     */
    bool _isGateway = false;
};