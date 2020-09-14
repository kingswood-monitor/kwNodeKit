/**
 * kwNode.cpp
 * Copyright (c) 2020 Richard J. Lyon
 *
 * See LICENSE for terms.
 */

#include <Arduino.h>
#include <pb_encode.h>

#include "debug.h"
#include "kwNode.h"
#include "packet.pb.h"

/** Maximum size of the buffer holding the encoded protobuf stream. */
#define MAX_PROTOBUF_BYTES 120
/** Maximum number of sensors. */
#define MAX_SENSORS 4
/** Maximum number of transports. */
#define MAX_TRANSPORTS 2

/** List and pointer for registering transports */
kwTransport *transports[MAX_TRANSPORTS];
uint8_t transportCount;

/** List and pointer for registering sensors */
kwSensor *sensors[MAX_SENSORS];
uint8_t sensorCount;

/** Flag for specifying "report by exception". */
bool g_rbeFlag;

/*-----------------------------------------------------------
 * CONSTRUCTORS
 *----------------------------------------------------------*/

kwNode::kwNode(){};

kwNode::kwNode(NodeLocation location, NodeType type, uint8_t chipId, const char *firmwareVersion)
{
    sensorCount = 0;

    // Build the metadata structure
    nodeMeta.node_location = location;
    nodeMeta.node_type = type;
    nodeMeta.chip_id = chipId;
    strcpy(nodeMeta.firmware_version, firmwareVersion);
}

/*-----------------------------------------------------------
 * PUBLIC METHODS
 *----------------------------------------------------------*/

Meta kwNode::meta()
{
    return nodeMeta;
}

void kwNode::addSensor(kwSensor *sensor)
{
    sensors[sensorCount++] = sensor;
}

void kwNode::addTransport(kwTransport *transport)
{
    transports[transportCount++] = transport;
}

void kwNode::start()
{
    for (int j = 0; j < transportCount; ++j)
    {
        transports[j]->startTransport();
    }

    for (int i = 0; i < sensorCount; ++i)
    {
        sensors[i]->startSensor();

        Serial.print(F("Starting Sensor "));
        Serial.print(i);
        Serial.print(F("..."));

        // Remove sensors that haven't started
        if (!sensors[i]->isInstalled())
        {
            Serial.println(F("failed: removed"));
            delete sensors[i];
            if (i < sensorCount)
            {
                for (int j = i; j < sensorCount; j++)
                {
                    sensors[j] = sensors[j + 1];
                }
            }
            sensorCount -= 1;
            i -= 1;
        }
        else
        {
            Serial.println("OK");
        }
    }
}

uint8_t kwNode::readAndEncodeMeasurements(uint16_t packetID, std::array<uint8_t, 255> &buffer, bool rbeFlag)
{
    g_rbeFlag = rbeFlag;

    // Create the stream to hold the encoded bytes
    pb_ostream_t ostream = pb_ostream_from_buffer(buffer.data(), MAX_PROTOBUF_BYTES);

    // Build the packet definition structure to pass to the encoder
    Packet packet = Packet_init_zero;
    packet.packet_id = packetID;
    packet.has_meta = true;
    packet.meta = meta();
    packet.measurements.funcs.encode = encodeMeasurements;

    // Encode the packet into the stream, or report failure
    if (!pb_encode(&ostream, Packet_fields, &packet))
    {
        Serial.print(F("E:"));
        Serial.println(ostream.errmsg);
        return ostream.bytes_written;
    }

    return ostream.bytes_written;
}

/*-----------------------------------------------------------
 * HELPERS
 *----------------------------------------------------------*/

bool encodeMeasurements(pb_ostream_t *ostream, const pb_field_iter_t *field, void *const *arg)
{
    // For each attached sensor
    for (int i = 0; i < sensorCount; ++i)
    {
        // read and encode the sensor's measurements
        sensors[i]->readAndEncodeMeasurements(ostream, field, arg, g_rbeFlag);
    }
    return true;
}