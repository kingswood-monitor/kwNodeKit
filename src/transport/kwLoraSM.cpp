/**
 * kwLoraSM.ch: Environment Sensor node firmware
 * Copyright (c) 2020 Richard J. Lyon
 * 
 * See LICENSE for terms.
 */
// https://randomnerdtutorials.com/esp32-lora-rfm95-transceiver-arduino-ide/

#include "kwLoraSM.h"
#include "packet.pb.h"
#include <string>

#include <SPI.h>
#include <LoRa.h>

using namespace std;

// TTGO LoRa pin assignments
#define PIN_CS 18
#define PIN_RST 14
#define PIN_INT 26

/* forward declarations */
void vPrintPacket(uint16_t uiTimeStamp, uint8_t *packetBuffer, uint8_t bytesWritten);

// constructors /////////////////////////////////////////////////////////////////////

kwLoraSM::kwLoraSM() : kwSensor(SensorName_LORA){};

// kwTransport interface ////////////////////////////////////////////////////////////

bool kwLoraSM::
    startTransport()
{
    LoRa.setPins(PIN_CS, PIN_RST, PIN_INT);
    while (!LoRa.begin(frequency_))
    {
        Serial.println(".");
        delay(500);
    }
    // LoRa.onReceive(onReceive);
    // LoRa.onTxDone(onTxDone);
    LoRa_rxMode();

    transportLed.blink(3);

    return true;
};

bool kwLoraSM::
    sendPacket(uint16_t uiTimeStamp, uint8_t *packetBuffer, uint8_t bytesWritten)
{
    LoRa_sendMessage(packetBuffer, bytesWritten);
    
    /* print debug info */
    vPrintPacket(uiTimeStamp, packetBuffer, bytesWritten);

    return true;
};

// kwSensor interface ///////////////////////////////////////////////////////////////

bool kwLoraSM::
    startSensor()
{
    isInstalled_ = true;
    return isInstalled_;
}
bool kwLoraSM::
    readAndEncodeMeasurements(
        pb_ostream_t *ostream,
        const pb_field_iter_t *field,
        void *const *arg,
        bool rbeFlag)
{
    bool result = false;

    Measurement measurement = Measurement_init_default;
    measurement.sensor = name_;

    // rssi
    result |= processMeasurement(
        measurement,
        LoRa.packetRssi(),
        rbeRSSIConfig,
        rbeFlag,
        Measurement_rssi_tag,
        "RSSI",
        ostream,
        field);

    // snr
    result |= processMeasurement(
        measurement,
        LoRa.packetSnr(),
        rbeSNRConfig,
        rbeFlag,
        Measurement_snr_tag,
        "SNR",
        ostream,
        field);

    // frequency error
    result |= processMeasurement(
        measurement,
        LoRa.packetFrequencyError(),
        rbeFrequencyErrorConfig,
        rbeFlag,
        Measurement_frequency_error_tag,
        "FE",
        ostream,
        field);

    return result;
}

// private methods //////////////////////////////////////////////////////////////////

void kwLoraSM::
    LoRa_rxMode()
{
    if (isGateway())
    {
        LoRa.disableInvertIQ(); // normal mode
        LoRa.receive();         // set receive mode
    }
    else
    {
        LoRa.enableInvertIQ(); // active invert I and Q signals
        LoRa.receive();        // set receive mode
    }
}

void kwLoraSM::
    LoRa_txMode()
{
    if (isGateway())
    {
        LoRa.idle();           // set standby mode
        LoRa.enableInvertIQ(); // active invert I and Q signals
    }
    else
    {
        LoRa.idle();            // set standby mode
        LoRa.disableInvertIQ(); // normal mode
    }
}

void kwLoraSM::
    onTxDone()
{
    Serial.println("TxDone");
    // LoRa_rxMode();
}

void kwLoraSM::
    LoRa_sendMessage(uint8_t *buffer, size_t size)
{
    LoRa_txMode();                             // set tx mode
    LoRa.beginPacket();                        // start packet
    LoRa.write((const uint8_t *)buffer, size); // add payload
    LoRa.endPacket(true);                      // finish packet and send it
}

uint8_t kwLoraSM::
    parsePacket(uint8_t *buffer)
{
    int packetSize = LoRa.parsePacket();
    if (packetSize)
    {
        // received a packet

        uint8_t i = 0;
        while (LoRa.available())
        {
            buffer[i++] = LoRa.read();
        }

        assert(i == packetSize);

        // Serial.print("Received packet: bytes=");
        // Serial.print(packetSize);
        // Serial.print(", RSSI=");
        // Serial.println(LoRa.packetRssi());
    }
    return packetSize;
}

// helper
void onReceive(int packetSize)
{
    // String message = "";

    // while (LoRa.available())
    // {
    //     message += (char)LoRa.read();
    // }

    // Serial.print("WOOOHOOOOOO   Node Receive: ");
    // Serial.println(message);
}

// helpers //////////////////////////////////////////////////////////////////////////////


/* utility function to print packet to serial */
void vPrintPacket(uint16_t uiTimeStamp, uint8_t *packetBuffer, uint8_t bytesWritten)
{
    char hexCar[2];
    
    Serial.print(F("\n["));
    Serial.print(uiTimeStamp);
    Serial.print(F("] SEND "));

    for (int i = 0; i < bytesWritten; i++)
        {
            sprintf(hexCar, "%02X", packetBuffer[i]);
            Serial.print(hexCar);
        }
    Serial.print(F(" ("));
    Serial.print(bytesWritten);
    Serial.println(F("B)"));
}