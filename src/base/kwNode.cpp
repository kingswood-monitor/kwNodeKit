#include "debug.h"

#include <Arduino.h>

#include <pb_encode.h>
#include "packet.pb.h"

#include "kwNode.h"

#define MAX_PROTOBUF_BYTES 120
#define MAX_TRANSPORTS 2
#define MAX_SENSORS 4

kwTransport *transports[MAX_TRANSPORTS];
kwSensor *sensors[MAX_SENSORS];

uint8_t sensorCount;
uint8_t transportCount;

bool g_rbeFlag;

kwNode::
    kwNode(){};

kwNode::
    kwNode(NodeLocation location, NodeType type, uint8_t chipId, const char *firmwareVersion)
{
    sensorCount = 0;

    meta_.node_location = location;
    meta_.node_type = type;
    meta_.chip_id = chipId;
    strcpy(meta_.firmware_version, firmwareVersion);
}

Meta kwNode::
    meta() { return meta_; }

void kwNode::
    addSensor(kwSensor *sensor) { sensors[sensorCount++] = sensor; }

void kwNode::
    addTransport(kwTransport *transport) { transports[transportCount++] = transport; }

void kwNode::
    start()
{
    for (int j = 0; j < transportCount; ++j)
    {
        transports[j]->startTransport();
    }

    for (int i = 0; i < sensorCount; ++i)
    {
        sensors[i]->startSensor();

        // remove sensors that haven't started
        if (!sensors[i]->isInstalled())
        {
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
    }
}

uint8_t kwNode::
    readAndEncodeMeasurements(uint16_t packetID, uint8_t *buffer, bool rbeFlag)
{
    g_rbeFlag = rbeFlag;

    // build packet
    Packet packet = Packet_init_zero;
    packet.packet_id = packetID;
    packet.has_meta = true;
    packet.meta = meta();
    packet.measurements.funcs.encode = encodeMeasurements;

    // encode packet
    pb_ostream_t ostream = pb_ostream_from_buffer(buffer, MAX_PROTOBUF_BYTES);
    if (!pb_encode(&ostream, Packet_fields, &packet))
    {
        Serial.print(F("E:"));
        Serial.println(ostream.errmsg);
        return ostream.bytes_written;
    }

    return ostream.bytes_written;
}

bool encodeMeasurements(pb_ostream_t *ostream, const pb_field_iter_t *field, void *const *arg)
{
    for (int i = 0; i < sensorCount; ++i)
    {
        // we've removed uninstalled sensors, so don't need to check if (sensors[i]->isInstalled())
        sensors[i]->readAndEncodeMeasurements(ostream, field, arg, g_rbeFlag);
    }
    return true;
}