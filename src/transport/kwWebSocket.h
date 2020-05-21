/**
 * kwWebSocket.h: Environment Sensor node firmware
 * Copyright (c) 2020 Richard J. Lyon
 * 
 * See LICENSE for terms.
 */

#include "kwTransport.h"
#include <ArduinoWebsockets.h>

class kwWebSocket : public kwTransport
{
public:
    // constructor
    kwWebSocket();
    kwWebSocket(
        const char *ssid,
        const char *password,
        WSInterfaceString host, int port,
        WSInterfaceString path)

        // kwTransport interface
        bool startTransport();
    bool sendPacket(uint16_t packetID, uint8_t *packetBuffer, uint8_t bytesWritten);
}