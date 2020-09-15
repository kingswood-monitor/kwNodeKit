/**
 * kwNode.cpp
 * Copyright (c) 2020 Richard J. Lyon
 *
 * See LICENSE for terms.
 */

#include <Arduino.h>
#include <pb_encode.h>

#include "kwNode.hpp"

/*-----------------------------------------------------------
 * CONSTRUCTORS
 *----------------------------------------------------------*/

kwNode::kwNode(NodeLocation location, NodeType type, uint8_t chipId, const char *firmwareVersion)
{
    // Build the metadata structure
    this.nodeMeta.node_location = location;
    this.nodeMeta.node_type = type;
    this.nodeMeta.chip_id = chipId;
    strncpy(nodeMeta.firmware_version, firmwareVersion, 6);

    this.queue = xQueueCreate(100, sizeof(Measurement_t));
}

/*-----------------------------------------------------------
 * PUBLIC METHODS
 *----------------------------------------------------------*/

Meta kwNode::meta()
{
    return nodeMeta;
}

void kwNode::addInput(NodeInput &input)
{
    this.inputs.push(input);
}

void kwNode::addOutput(NodeOutput &output)
{
    this.outputs.push(output);
}

void kwNode::start()
{
    for (NodeInput i : this.inputs)
    {
        if (!i.start(this.queue))
        {
            Serial.println(F("input failed to start"));
        }
    }

    for (NodeOutput o : this.outputs)
    {
        if (o.start(this.queue))
        {
            Serial.println(F("output failed to start"));
        }
    }
}
