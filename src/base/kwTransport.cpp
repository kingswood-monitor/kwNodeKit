#include "kwTransport.h"
#include "kwLED.h"

kwTransport::
    kwTransport()
{
    pinMode(LED_BUILTIN, OUTPUT);
    transportLed = kwLED();
};

void kwTransport::
    isGateway(bool flag) { _isGateway = flag; }

bool kwTransport::
    isGateway() { return _isGateway; }