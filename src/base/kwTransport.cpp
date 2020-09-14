#include "kwTransport.h"
#include "kwLED.h"

kwTransport::kwTransport(){};

void kwTransport::isGateway(bool flag)
{
    _isGateway = flag;
}

bool kwTransport::isGateway()
{
    return _isGateway;
}