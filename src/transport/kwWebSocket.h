/**
 * kwWebSocket.h: Environment Sensor node firmware
 * Copyright (c) 2020 Richard J. Lyon
 * 
 * See LICENSE for terms.
 */

#include "kwTransport.h"
#include <ArduinoWebsockets.h>
#include <WiFi.h>

class kwWebSocket : public kwTransport
{
public:
    // constructor
    kwWebSocket(
        const char *ssid,
        const char *password,
        const char *websockets_server_host,
        const uint16_t websockets_server_port,
        String path);

    // kwTransport interface
    bool startTransport();
    bool sendPacket(uint8_t *packetBuffer, uint8_t bytesWritten);

private:
    const char *_ssid = {nullptr};
    const char *_password = {nullptr};
    const char *_websockets_server_host = {nullptr};
    const uint16_t _websockets_server_port = {0};
    String _path = "/";
};