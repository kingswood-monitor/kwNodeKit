#pragma once
/**
 * kwNode.h
 * Copyright (c) 2020 Richard J. Lyon
 * 
 * See LICENSE for terms.
 */

#include "kwSensor.h"
#include "kwTransport.h"
#include "packet.pb.h"

/**
 * Defines a hardware device for measuring and reporting environment information having a number of sensors and transmitters, 
 * the ability to encode its sensor information as a protobuf byte stream, and metadata describing its name and location.
 */
class kwNode
{
public:
    /*-----------------------------------------------------------
     * CONSTRUCTORS
     *----------------------------------------------------------*/

    /** Default constructor. */
    kwNode();

    /**
     * Construct a new node with a specified location, type, id, and firmware version. These are 
     * typically used downstream to identify the node. These is packed by the contructor into the meta_
     * private variable, which is a structure defined in the protobuf protocol.
     * 
     * @param location The location of the node as defined in the protobuf _NodeLocation 
     * specification e.g. 'NodeLocation_greenhouse'
     * 
     * @param type The type of the node as defined in the protobuf _NodeType 
     * specification e.g. 'NodeType_bridge'
     * 
     * @param chipid A value that identifies the id of the MCU.
     * 
     * @param firmwareVersion The firmware version.
     */
    kwNode(
        NodeLocation location,
        NodeType type,
        uint8_t chipId,
        const char *firmwareVersion);

    /*-----------------------------------------------------------
     * PUBLIC METHODS
     *----------------------------------------------------------*/

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
    void addSensor(kwSensor *sensor);

    /** 
     * @brief Add a transport to the list of transports. 
     * Transports are maintained in an array of pointers to transport objects and a counter.
     * 
     * @param transport Pointer to the transport object to add.
     */
    void addTransport(kwTransport *transport);

    /**
     * @brief Start the sensors and transport.
     * 
     * Iterates through each of the attached sensors and transports and calls their startSensor() or 
     * startTransport() methods. Sensors that fail to start (i.e. isInstalled() returns FALSE) are removed 
     * from the sensor list.
     * 
     * FIXME Need to add this removal process to transports that fail to start. 
     */
    void start();

    /**
     * @brief Read the attached sensors and encode the measurements to a protobuf byte stream.
     * 
     * The function inserts the data to be encoded into a Packet structure defined by the protobuf
     * protocol. This data comprises a packet id number, metadata about the node, and the measurements
     * from the attached sensors. The structure is passed to the protobuf encoding function, which 
     * stores the encoded byte stream in a supplied buffer. To save space, data that has not changed 
     * since last encoding can be omitted. The function returns the number of bytes
     * in the ecoded stream.
     * 
     * @param packetID A number allowing the packet to be identified.
     * 
     * @param buffer A buffer for storing the encoded byte stream.
     * 
     * @param rbeFlag A boolean flag specifying whether to compress measurement data ("report by exception").
     * 
     * @return The number of bytes written to the encoded stream.
     */
    uint8_t readAndEncodeMeasurements(
        uint16_t packetID,
        uint8_t *buffer,
        bool rbeFlag);

private:
    /*-----------------------------------------------------------
     * PRIVATE PROPERTIES
     *----------------------------------------------------------*/

    /** 
     * Holds the node metadata in a structure defined in the protobuf protocol. It's initialsed to defaults
     * also defined in the protocol.
     */
    Meta meta_ = Meta_init_default;
};

/*-----------------------------------------------------------
 * HELPERS
 *----------------------------------------------------------*/

/**
 * The function passed to the encoder that actually encodes the individual sensor measurements. It
 * iterates through the list of attached sensors and calls its readAndEncodeMeasurements() function. 
 * This function provides the sensor-specific encoding information i.e. sensor name, measurement name, and 
 * measurement value. For arguments, see the nanopb protobuf encoding library.
 */
bool encodeMeasurements(
    pb_ostream_t *ostream,
    const pb_field_iter_t *field,
    void *const *arg);
