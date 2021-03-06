/**
 * kwLoraSM.h
 * Copyright (c) 2020 Richard J. Lyon
 *
 * See LICENSE for terms.
 */
#include <Arduino.h>
#include <string>

#include <LoRa.h>
#include <SPI.h>

#include "kwLoraSM.h"
#include "packet.pb.h"

using namespace std;

// TTGO LoRa pin assignments
#define PIN_CS 18
#define PIN_RST 14
#define PIN_INT 26

// Signal LED pin
const uint8_t SIGNAL_LED = 25;

// Depth of the stack for FreeRTOS tasks.
#define STACK_DEPTH 3000

// Semaphore to coordinate ISR and lora handler task.
SemaphoreHandle_t binarySemaphore = xSemaphoreCreateBinary();

// Timer to manage LED flashing
TimerHandle_t LEDFlashTimer;

// Global variable for passing the received packet size from the interrupt handler to the service task.
uint16_t gPacketSize;

/* forward declarations */
void vPrintPacket(uint16_t uiTimeStamp, uint8_t *packetBuffer, uint8_t bytesWritten);

/*-----------------------------------------------------------
 * CONSTRUCTORS
 *----------------------------------------------------------*/

kwLoraSM::kwLoraSM() : kwSensor(SensorName_LORA){};

/*-----------------------------------------------------------
 * kwTransport VIRTUAL INTERFACE METHODS
 *----------------------------------------------------------*/

bool kwLoraSM::startTransport()
{
    /**
     * Start the radio. WARNING: this will hang the device if the radio can't be started.
     * FIXME turn this into a timed operation and graceful quite.
     */
    Serial.print("Starting LoRa...");

    LoRa.setPins(PIN_CS, PIN_RST, PIN_INT);
    txPower_ = DEFAULT_POWER;
    setTxPower(txPower_);

    while (!LoRa.begin(frequency_))
    {
        Serial.print(".");
        delay(500);
    }

    Serial.println("started");

    //  Place the device in receive mode
    LoRa_rxMode();

    //  Set the interrupt handler for received messages
    LoRa.onReceive(onReceive);

    // Create the LED flash timer
    LEDFlashTimer = xTimerCreate("LEDFlash", pdMS_TO_TICKS(50), pdFALSE, 0, LEDOneShotTimerCallback);

    //  Assume if we get here, the device has started
    return true;
};

bool kwLoraSM::sendPacket(uint16_t uiTimeStamp, uint8_t *packetBuffer, uint8_t bytesWritten)
{
    //  NOTE: We are ignoring uiTimeStamp
    sendPacket(packetBuffer, bytesWritten);

    /* Print debug info FIXME Add a debug flag */
    vPrintPacket(uiTimeStamp, packetBuffer, bytesWritten);

    return true;
};

/*-----------------------------------------------------------
 * kwSensor VIRTUAL INTERFACE METHODS
 *----------------------------------------------------------*/

bool kwLoraSM::startSensor()
{
    //  FIXME This restarts the LoRa radio to see if it is working - check if this has side effects.
    bool bDidStart = LoRa.begin(frequency_);
    isInstalled(bDidStart);

    return bDidStart;
}

bool kwLoraSM::readAndEncodeMeasurements(pb_ostream_t *ostream, const pb_field_iter_t *field, void *const *arg,
                                         bool rbeFlag)
{
    //  Flag to capture that encoding was successful
    bool bSuccess = false;

    //  Define a measurement with default values and set the sensor name.
    Measurement measurement = Measurement_init_default;
    measurement.sensor = name_;

    //  Get the RSSI
    float rssi = LoRa.packetRssi();

    //  Encode it and capture the success
    bSuccess |=
        processMeasurement(measurement, rssi, rbeRSSIConfig, rbeFlag, Measurement_rssi_tag, "RSSI", ostream, field);

    ///  Get the SNR
    float snr = LoRa.packetSnr();

    //  Encode it and capture the success
    bSuccess |= processMeasurement(measurement, LoRa.packetSnr(), rbeSNRConfig, rbeFlag, Measurement_snr_tag, "SNR",
                                   ostream, field);

    //  Get the frequency error
    float frequencyError = LoRa.packetFrequencyError();

    //  Encode it and capture the success
    bSuccess |= processMeasurement(measurement, frequencyError, rbeFrequencyErrorConfig, rbeFlag,
                                   Measurement_frequency_error_tag, "FE", ostream, field);

    return bSuccess;
}

/*-----------------------------------------------------------
 * PUBLIC METHODS
 *----------------------------------------------------------*/

void kwLoraSM::sendPacket(uint8_t *buffer, size_t size)
{
    LoRa_txMode();                             // set tx mode
    LoRa.beginPacket();                        // start packet
    LoRa.write((const uint8_t *)buffer, size); // add payload
    LoRa.endPacket(true);                      // finish packet and send it
}

uint8_t kwLoraSM::parsePacket(uint8_t *buffer)
{
    int packetSize = LoRa.parsePacket();
    if (packetSize)
    {
        uint8_t i = 0;
        while (LoRa.available())
        {
            buffer[i++] = LoRa.read();
        }

        assert(i == packetSize);
    }
    return packetSize;
}

void kwLoraSM::startInterruptHandling()
{
    if (binarySemaphore != nullptr)
    {
        xTaskCreate(loraHandlerTask, "LoRa Handler", STACK_DEPTH, nullptr, 3, nullptr);
    }
}

void kwLoraSM::setTxPower(uint8_t txPower)
{
    LoRa.setTxPower(txPower);
}

int16_t kwLoraSM::rssi()
{
    return LoRa.packetRssi();
}

int16_t kwLoraSM::snr()
{
    return LoRa.packetSnr();
}

int16_t kwLoraSM::freqError()
{
    return LoRa.packetFrequencyError();
}

/*-----------------------------------------------------------
 * PRIVATE METHODS
 *----------------------------------------------------------*/

void kwLoraSM::LoRa_rxMode()
{
    /**
     * We use I/Q signal inversion to create gateway mode.
     * See e.g.
     * https://github.com/sandeepmistry/arduino-LoRa/blob/master/examples/LoRaSimpleGateway/LoRaSimpleGateway.ino
     */
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

void kwLoraSM::LoRa_txMode()
{
    /**
     * We use I/Q signal inversion to create gateway mode.
     * See e.g.
     * https://github.com/sandeepmistry/arduino-LoRa/blob/master/examples/LoRaSimpleGateway/LoRaSimpleGateway.ino
     */
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

void kwLoraSM::onTxDone()
{
    Serial.println("TxDone");
    // LoRa_rxMode();
}

/*-----------------------------------------------------------
 * CALLBACKS
 *----------------------------------------------------------*/

void LEDOneShotTimerCallback(TimerHandle_t xTimer)
{
    digitalWrite(SIGNAL_LED, LOW);
}

void onReceive(int packetSize)
{
    gPacketSize = packetSize;
    Serial.print("onReceive: Fired, packet size ");
    Serial.println(gPacketSize);

    digitalWrite(BUILTIN_LED, LOW);

    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(binarySemaphore, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR();
    // portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/*-----------------------------------------------------------
 * HELPERS
 *----------------------------------------------------------*/

void loraHandlerTask(void *pvParameters)
{
    while (true)
    {
        Serial.println("loraHandlerTask: Handling event");

        xSemaphoreTake(binarySemaphore, portMAX_DELAY);

        // if this task has been scheduled, there should be a packet
        if (gPacketSize > 0)
        {
            // get the packet
            uint8_t buffer[gPacketSize];
            uint8_t i = 0;
            while (LoRa.available())
            {
                buffer[i++] = LoRa.read();
            }

            assert(i == gPacketSize);

            vPrintPacket(0, buffer, gPacketSize);

            // flash the led
            if (LEDFlashTimer != NULL)
            {
                digitalWrite(SIGNAL_LED, HIGH);
                xTimerStart(LEDFlashTimer, 0);
            }

            // TODO: post the buffer in the mailbox
        }
        else
        {
            Serial.println();
        }
    }
}

void vPrintPacket(uint16_t uiTimeStamp, uint8_t *packetBuffer, uint8_t bytesWritten)
{
    char hexCar[2];

    Serial.print(F("\n["));
    Serial.print(uiTimeStamp);
    Serial.print(F("] RECEIVE "));

    for (int i = 0; i < bytesWritten; i++)
    {
        sprintf(hexCar, "%02X", packetBuffer[i]);
        Serial.print(hexCar);
    }
    Serial.print(F(" ("));
    Serial.print(bytesWritten);
    Serial.println(F("B)"));
}