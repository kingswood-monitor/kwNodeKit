/**
 * simpleNode.cpp
 * 
 * Example of a simple NodeKit node with a DH22 temperature and humidity sensor
 * and LoRa transport.
 * 
 * Transmits temperature and humidity every second to a receiver.
 */

#include <kwNode.h>
#include <kwDHT22.h>
#include <kwLoraSM.h>

#include "packet.pb.h"

#include "util.h"

#define LOCATION NodeLocation_greenhouse
#define TYPE NodeType_environment
#define CHIPID 123
#define FIRMWARE_VERSION "1.0.0"

#define DH22_DATA_PIN 16

// create the node
kwNode node(LOCATION, TYPE, CHIPID, FIRMWARE_VERSION);

// create the transport
kwLoraSM lora;

// creste the sensor
kwDHT22 *pDHT22 = new kwDHT22(DH22_DATA_PIN);

void setup()
{
    node.addSensor(pDHT22);
    node.addTransport(&lora);
    node.start();
}

uint16_t packetID = 0;
uint8_t packetBuffer[255] = {0};

void loop()
{
    if (runEvery(1000))
    {
        uint8_t bytesWritten = node.readAndEncodeMeasurements(packetID++, packetBuffer, true);
        lora.sendPacket(packetBuffer, bytesWritten);
    }
}