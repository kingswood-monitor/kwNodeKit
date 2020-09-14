/**
 * kwTransport.h
 * Copyright (c) 2020 Richard J. Lyon
 * 
 * See LICENSE for terms.
 */

#include "kwTransport.h"

/*-----------------------------------------------------------
 * CONSTRUCTORS
 *----------------------------------------------------------*/

kwTransport::kwTransport(){};

/*-----------------------------------------------------------
 * PUBLIC METHODS
 *----------------------------------------------------------*/

void kwTransport::isGateway(bool flag)
{
    _isGateway = flag;
}

bool kwTransport::isGateway()
{
    return _isGateway;
}