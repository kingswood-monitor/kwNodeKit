#include "kwWebSocket.h"

void onMessageCallback(WebsocketsMessage message);
// void onEventsCallback(WebsocketsEvent event, String data);

kwWebSocket::
    kwWebSocket(
        const char *ssid,
        const char *password,
        const char *websockets_server_host,
        const uint16_t websockets_server_port)
    : _ssid(ssid), _password(password), _websockets_server_host(websockets_server_host), _websockets_server_port(websockets_server_port)
{
}

bool kwWebSocket::
    startTransport()
{
    Serial.print("INFO: Connecting to WiFi");
    WiFi.begin(_ssid, _password);

    for (int i = 0; i < 10 && WiFi.status() != WL_CONNECTED; i++)
    {
        Serial.print(".");
        delay(1000);
    }
    Serial.print("Connected, IP: ");
    Serial.println(WiFi.localIP());

    // run callback when messages are received
    _client.onMessage(onMessageCallback);

    // run callback when events are occuring
    // client.onEvent(onEventsCallback);

    // connect to websocket server
    Serial.print("INFO: Connecting to websocket");
    while (!_client.available())
    {
        _client.connect(_websockets_server_host, _websockets_server_port, "/");
        delay(1000);
    }
    Serial.println();

    // Send a message
    _client.send("Hello Server");

    // Send a ping
    _client.ping();

    return true;
}

bool kwWebSocket::
    sendPacket(uint16_t uiTimeStamp, uint8_t *packetBuffer, uint8_t bytesWritten)
{
    while (!_client.available())
    {
        _client.connect(_websockets_server_host, _websockets_server_port, "/");
        delay(1000);
    }

    _client.sendBinary((char *)packetBuffer, bytesWritten);

    return true;
}

/****************************************************************************
 * Websocket callbacks                                                      */

void onMessageCallback(WebsocketsMessage message)
{
    Serial.print("Got Message: ");
    Serial.println(message.data());
}

// void onEventsCallback(WebsocketsEvent event, String data, const char *websockets_server_host, const uint16_t websockets_server_port)
// {
//     if (event == WebsocketsEvent::ConnectionOpened)
//     {
//         Serial.println("INFO: Connnection Opened");
//     }

//     else if (event == WebsocketsEvent::ConnectionClosed)
//     {
//         Serial.println("INFO: Connnection Closed");
//         client.connect(websockets_server_host, websockets_server_port, "/");
//     }

//     else if (event == WebsocketsEvent::GotPing)
//     {
//         Serial.println("INFO: Got a Ping!");
//     }

//     else if (event == WebsocketsEvent::GotPong)
//     {
//         Serial.println("INFO: Got a Pong!");
//     }
// }