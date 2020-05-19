#pragma once
/**
 * kwNodeLite.h: Environment Sensor node firmware
 * Copyright (c) 2020 Richard J. Lyon
 * 
 * See LICENSE for terms.
 */

#include "packet.pb.h"
#include "kwSensor.h"
#include "kwTransport.h"

class kwNode
{
public:
    kwNode();
    kwNode(NodeLocation location, NodeType type, uint8_t chipId, const char *firmwareVersion);

    // getters
    Meta meta();

    // add a sensor to the list of sensors
    void addSensor(kwSensor *sensor);

    // add a transport
    void addTransport(kwTransport *transport);

    // start sensors and tranport
    void start();

    //read sensors and encode to protobuf
    uint8_t readAndEncodeMeasurements(uint16_t packetID, uint8_t *buffer, bool rbeFlag);

private:
    Meta meta_ = Meta_init_default;
};

bool encodeMeasurements(pb_ostream_t *ostream, const pb_field_iter_t *field, void *const *arg);