#include "kwTransport.h"
#include "kwLED.h"

kwTransport::
    kwTransport()
{
    pinMode(LED_BUILTIN, OUTPUT);
    transportLed = kwLED();
};