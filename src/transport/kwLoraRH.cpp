/**
 * kwLora.cpp: Environment Sensor node firmware
 * Copyright (c) 2020 Richard J. Lyon
 *
 * See LICENSE for terms.
 */

#include <RHReliableDatagram.h>
#include <RH_RF95.h>

#include "kwLoraRH.h"
#include "packet.pb.h"

// feather32u4 LoRa pin assignments
#if defined(ARDUINO_AVR_FEATHER32U4)
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 7
#elif defined(ARDUINO_ARCH_ESP32)
#define RFM95_CS 18
#define RFM95_RST 14
#define RFM95_INT 26
#else
#error Unsupported board
#endif

RH_RF95 driver(RFM95_CS, RFM95_INT);
RHReliableDatagram manager(driver, 0);

// Constructors
kwLoraRH::kwLoraRH() : kwSensor(SensorName_LORA){};

kwLoraRH::kwLoraRH(uint8_t clientAddress, uint8_t txPower) : kwSensor(SensorName_LORA), txPower_(txPower)
{
    manager.setThisAddress(clientAddress);
};

// kwTransport interface

bool kwLoraRH::startTransport()
{
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);

    bool status = driver.init();

    Serial.println("Starting LoRa");
    Serial.println(status);

    RH_RF95::ModemConfig modemConfig = {RH_RF95_BW_15_6KHZ | RH_RF95_CODING_RATE_4_8, RH_RF95_SPREADING_FACTOR_512CPS,
                                        0x04};

    driver.setTxPower(10);
    driver.setFrequency(868.0);
    driver.setModemRegisters(&modemConfig);

    // driver.setTxPower(txPower_);
    // driver.setFrequency(frequency_);
    // driver.setModemConfig(RH_RF95::Bw500Cr45Sf128);

    if (!manager.init())
    {
        return false;
    }

    digitalWrite(LED_BUILTIN, LOW);
    return true;
}

uint8_t ok[3] = "OK";
uint8_t ok_len = 3;

bool kwLoraRH::sendPacket(uint16_t packetID, uint8_t *packetBuffer, uint8_t bytesWritten)
{
    if (manager.sendtoWait(packetBuffer, bytesWritten, serverAddress_))
    {
        Serial.print(F("["));
        Serial.print(packetID);
        Serial.print(F("] ->0x"));
        Serial.print(serverAddress_);
        Serial.print(" (");
        Serial.print(bytesWritten);
        Serial.print(F("B) <- "));

        digitalWrite(LED_BUILTIN, HIGH);

        if (manager.recvfromAckTimeout(ok, &ok_len, 2000))
        {
            Serial.println((char *)ok);

            digitalWrite(LED_BUILTIN, LOW);
            // driver.sleep();

            return true;
        }
        else
        {
            Serial.print(F("[FAIL] No reply, is server running?"));
            return false;
        }
    }
    else
    {
        Serial.print(F("[FAIL] SendtoWait: Server 0x"));
        Serial.println(serverAddress_);
        return false;
    }
}

// kwSensor interface

bool kwLoraRH::startSensor()
{
    isInstalled_ = true;
    return isInstalled_;
}
bool kwLoraRH::readAndEncodeMeasurements(pb_ostream_t *ostream, const pb_field_iter_t *field, void *const *arg,
                                         bool rbeFlag)
{
    bool result = false;

    Measurement measurement = Measurement_init_default;
    measurement.sensor = name_;

    // rssi
    result |=
        processMeasurement(measurement, rssi(), rbeRSSIConfig, rbeFlag, Measurement_rssi_tag, "RSSI", ostream, field);

    // snr
    result |= processMeasurement(measurement, snr(), rbeSNRConfig, rbeFlag, Measurement_snr_tag, "SNR", ostream, field);

    // frequency error
    result |= processMeasurement(measurement, freqError(), rbeFrequencyErrorConfig, rbeFlag,
                                 Measurement_frequency_error_tag, "FE", ostream, field);

    return result;
}

// methods

int16_t kwLoraRH::rssi()
{
    return driver.lastRssi();
}

int16_t kwLoraRH::snr()
{
    return driver.lastSNR();
}

int16_t kwLoraRH::freqError()
{
    return driver.frequencyError();
}

// utility functions

void printHex(uint8_t num)
{
    char hexCar[2];

    sprintf(hexCar, "%02X", num);
    Serial.print(hexCar);
}

void printPacket(const char *prompt, uint16_t packetID, uint8_t *packetBuffer, uint8_t bytesWritten)
{
    Serial.println();

    Serial.print(F("["));
    Serial.print(packetID);
    Serial.print(F("]"));

    Serial.print(F("Bytes:"));
    Serial.print(bytesWritten);
    Serial.print(F(" "));

    for (int i = 0; i < bytesWritten; i++)
    {
        printHex(packetBuffer[i]);
    }

    Serial.println();
}
