#pragma once

#include <Arduino.h>

void vReadMeasurementTask(void *pvParameters);
void vTransmitMeasurementTask(void *pvParameters);
void vCreateMailbox();
void vCreateLEDFlashTimer();

#define STACK_DEPTH 3000