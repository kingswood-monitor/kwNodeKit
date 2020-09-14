/**
 *  tasks.cpp
 *
 *  Realtime task and timer functions.
 */

#include "tasks.h"

#include <array>
#include <kwLoraSM.h>
#include <kwNode.h>

// #include "util.h" // for debugging "printpacket"

// Configure realtime intervals
const uint8_t SAMPLE_INTERVAL_SECS = 1;
const uint8_t TRANSMIT_INTERVAL_SECS = 5;

// Maximum length of packet buffer
const uint8_t MAX_BUFFER_LENGTH = 255;

TimerHandle_t xLEDFlashTimer;
BaseType_t xLEDFlashTimerStarted;
QueueHandle_t xMailbox;

// Structure for passing timestamped measurements from read to transmit tasks
typedef struct xMeasurementStructure
{
    TickType_t xTimeStamp = 0;
    std::array<uint8_t, MAX_BUFFER_LENGTH> packetBuffer;
    uint8_t bytesWritten = 0;
} Measurement_t;

/* forward declarations */
void vUpdateMailbox(std::array<uint8_t, 255> &buffer, uint8_t bytesWritten);
BaseType_t xReadMailbox(Measurement_t *pxData);
void prvLEDOneShotTimerCallback(TimerHandle_t xTimer);
void vPrintMeasurement();

/* Read measurements task */
void vReadMeasurementTask(void *pvParameters)
{
    kwNode *node = (kwNode *)pvParameters;

    TickType_t xLastWakeTime = xTaskGetTickCount();
    std::array<uint8_t, 255> packetBuffer;
    uint8_t bytesWritten;
    uint32_t packetID = 0;
    bool bCompressData = false;

    /* compute sample interval ticks */
    const uint16_t xSampleIntervalMS = pdMS_TO_TICKS(SAMPLE_INTERVAL_SECS * 1000);

    while (true)
    {
        /* read and encode measurements from active sensors */
        packetID = xLastWakeTime;
        bytesWritten = node->readAndEncodeMeasurements(packetID, packetBuffer, bCompressData);

        /* post measurements to mailbox */
        vUpdateMailbox(packetBuffer, bytesWritten);

        /* print debug info */
        vPrintMeasurement();

        /* enter block state */
        vTaskDelayUntil(&xLastWakeTime, xSampleIntervalMS);
    }
}

/* Transmit measurements task */
void vTransmitMeasurementTask(void *pvParameters)
{
    kwLoraSM *lora = (kwLoraSM *)pvParameters;

    Measurement_t xData;
    TickType_t xLastWakeTime = xTaskGetTickCount();

    /* compute transmit interval ticks */
    const uint16_t xTransmitIntervalMS = pdMS_TO_TICKS(TRANSMIT_INTERVAL_SECS * 1000);

    while (true)
    {
        /* get latest measurements from the mailbox */
        xReadMailbox(&xData);

        /* transmit packet */
        lora->sendPacket((uint16_t)xLastWakeTime, xData.packetBuffer.data(), xData.bytesWritten);

        /* flash LED */
        if (xLEDFlashTimer != NULL)
        {
            digitalWrite(LED_BUILTIN, HIGH);
            xLEDFlashTimerStarted = xTimerStart(xLEDFlashTimer, 0);
        }

        /* enter block state */
        vTaskDelayUntil(&xLastWakeTime, xTransmitIntervalMS);
    }
}

/* Update the mailbox with timestamped measurement data */
void vUpdateMailbox(std::array<uint8_t, 255> &buffer, uint8_t bytesWritten)
{
    Measurement_t xData;

    xData.xTimeStamp = xTaskGetTickCount();
    xData.packetBuffer = buffer;
    xData.bytesWritten = bytesWritten;

    xQueueOverwrite(xMailbox, &xData);
}

/* Read the current measurement from the mailbox, returning pdTRUE if changed */
BaseType_t xReadMailbox(Measurement_t *pxData)
{
    TickType_t xPreviousTimeStamp = 0;
    BaseType_t xDataUpdated = 0;

    xPreviousTimeStamp = pxData->xTimeStamp;
    xQueuePeek(xMailbox, pxData, portMAX_DELAY);

    if (pxData->xTimeStamp > xPreviousTimeStamp)
    {
        xDataUpdated = pdTRUE;
    }
    else
    {
        xDataUpdated = pdFALSE;
    }

    return xDataUpdated;
}

/* Create a mailbox for passing measurements from read to transmit tasks */
void vCreateMailbox()
{
    xMailbox = xQueueCreate(1, sizeof(Measurement_t));
}

/* Create a oneshot timer to flash the LED */
void vCreateLEDFlashTimer()
{
    xLEDFlashTimer = xTimerCreate("LEDFlash", pdMS_TO_TICKS(100), pdFALSE, 0, prvLEDOneShotTimerCallback);
}

/* Callback to turn the LED off */
void prvLEDOneShotTimerCallback(TimerHandle_t xTimer)
{
    digitalWrite(LED_BUILTIN, LOW);
}

/* Utility function to print mailbox content to serial */
// void vPrintMeasurement()
// {
//     Measurement_t xData;
//     xReadMailbox(&xData);

//     Serial.print(F("\n["));
//     Serial.print(xData.xTimeStamp);
//     Serial.print(F("] READ "));
//     for (int i = 0; i < xData.bytesWritten; i++)
//     {
//         printHex(xData.packetBuffer[i]);
//     }
//     Serial.print(F(" ("));
//     Serial.print(xData.bytesWritten);
//     Serial.println(F("B)"));
// }