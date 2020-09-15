/**
 * kwNode.h
 * Copyright (c) 2020 Richard J. Lyon
 *
 * See LICENSE for terms.
 */

#pragma once

#include <array>

#include "NodeInput.hpp"
#include "NodeOutput.hpp"
#include "packet.pb.h"

/**
 * Defines a hardware device for measuring and reporting environment information having a number of sensors and
 * transmitters, the ability to encode its sensor information as a protobuf byte stream, and metadata describing its
 * name and location.
 */
class kwNode
{
  public:
    /**
     * Construct a new node with a specified location, type, id, and firmware version. These are
     * typically used downstream to identify the node.
     *
     * @param location The location of the node as defined in the protobuf _NodeLocation
     * specification e.g. 'NodeLocation_greenhouse'
     *
     * @param type The type of the node as defined in the protobuf _NodeType
     * specification e.g. 'NodeType_bridge'
     *
     * @param chipid A value that identifies the id of the MCU.
     *
     * @param firmwareVersion The firmware version. Note only the first 6 chars will be used.
     */
    kwNode(NodeLocation location, NodeType type, uint8_t chipId, const char *firmwareVersion);

    /**
     * @brief Getter function returning the node's metadata.
     *
     * @return Meta
     * */
    Meta meta();

    /**
     * @brief Add a sensor to the list of sensors.
     * Sensors are maintained in an array of pointers to sensor objects and a counter.
     *
     * @param sensor Pointer to the sensor object to add.
     */
    void addInput(NodeInput &input);

    /**
     * @brief Add an output to the list of outputs.
     *
     * @param transport Pointer to the transport object to add.
     */
    void addOutput(NodeOutput &output);

    /**
     * @brief Start the node, kicking off the inputs and outputs.
     */
    void start();

  private:
    Meta nodeMeta = Meta_init_default;
    std::vector<NodeInput> inputs;
    std::vector<NodeOutput> outputs;
    QueueHandle_t queue;
};
