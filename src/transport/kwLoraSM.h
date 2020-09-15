/**
 * kwLoraSM.h
 * Copyright (c) 2020 Richard J. Lyon
 *
 * See LICENSE for terms.
 */

#include "LoRa.h"
#include "kwSensor.h"
#include "kwTransport.h"

//  Default LoRa transmit power
#define DEFAULT_POWER 17

/**
 * Defines a LoRa transport using the Sandeep Mistry LoRa library.
 * See: https://github.com/sandeepmistry/arduino-LoRa
 * See: https://randomnerdtutorials.com/esp32-lora-rfm95-transceiver-arduino-ide/
 */
class kwLoraSM : public kwTransport, public kwSensor
{
  public:
    /*-----------------------------------------------------------
     * CONSTRUCTORS
     *----------------------------------------------------------*/

    /** Default constructor. */
    kwLoraSM();

    /*-----------------------------------------------------------
     * kwTransport VIRTUAL INTERFACE METHODS
     *----------------------------------------------------------*/

    /**
     * @brief Initialise the radio.
     * This sets the pins, so check they are correct for the device. The device is set up as a
     * receiver, and an interrupt handler configured.
     */
    bool startTransport();

    /**
     * Send a packet of information over the channel.
     *
     * @param uiTimeStamp A number allowing the packet to be identified.
     * @param packetBuffer A pointer to a buffer containing the information to be transmitted.
     * @param bytesWritten The number of bytes to transmit.
     */
    bool sendPacket(uint16_t uiTimeStamp, uint8_t *packetBuffer, uint8_t bytesWritten);

    /*-----------------------------------------------------------
     * kwSensor VIRTUAL INTERFACE METHODS
     *----------------------------------------------------------*/

    /**
     * @brief Start the sensor.
     * The concrete implementation carries out any functions required to initialise
     * and start the sensor.
     *
     * @return TRUE if the sensor started.
     */
    bool startSensor();

    /**
     * @brief Read and encode the sensor.
     * This function is provided by the protobuf library for encoding the sensor's information.
     * It's how a sensor describes what it is e.g. 'LoRa', and the value. Each concrete
     * implementation provides the code required to read the sensor, and adds the metadata.
     *
     * @param pb_ostream_t The stream to encode readings to (see nanopb).
     * @param field The field to encode (see nanopb).
     * @param arg Arguments to the encoding process (see nanopb).
     * @param rbeFlag Set TRUE to specify Report By Exception processing.
     *
     * @return TRUE if the measurement encoded correctly.
     */
    bool readAndEncodeMeasurements(pb_ostream_t *ostream, const pb_field_iter_t *field, void *const *arg, bool rbeFlag);

    /*-----------------------------------------------------------
     * PUBLIC METHODS
     *----------------------------------------------------------*/

    /**
     * Send a packet from a buffer.
     *
     * @param buffer Buffer containing the packet to send
     * @param size The length in bytes of the packet
     */
    void sendPacket(uint8_t *buffer, size_t size);

    /**
     * Parse a data packet into a buffer.
     *
     * @param buffer A buffer to receive the packet.
     * @return The number of bytes received.
     */
    uint8_t parsePacket(uint8_t *buffer);

    void startInterruptHandling();

    /**
     * Set transmit power.
     * @param txPower The power.
     */
    void setTxPower(uint8_t txPower);

    /** Get the RSSI */
    int16_t rssi();

    /** Get the SNR */
    int16_t snr();

    /** Get the SNR */
    int16_t freqError();

  private:
    /*-----------------------------------------------------------
     * PRIVATE METHODS
     *----------------------------------------------------------*/

    /** Set receive mode */
    void LoRa_rxMode();

    /** Set transmit mode */
    void LoRa_txMode();

    /**
     * Send a message in buffer of given size/
     * @param buffer The buffer to send
     * @param size The length of the buffer
     */

    static void onTxDone();

    /*-----------------------------------------------------------
     * PRIVATE PROPERTIES
     *----------------------------------------------------------*/

    /** Transmit power */
    uint8_t txPower_ = {DEFAULT_POWER}; // valid levels +2 to +20

    /** Transmit frequency */
    double frequency_ = {868E6};
};

/*-----------------------------------------------------------
 * CALLBACKS
 *----------------------------------------------------------*/

/**
 * Interrupt Service Routine callback function after receiving a packet.
 *
 * @param packetSize The size of the received packet.
 */
void onReceive(int packetSize);

/* Callback to turn the LED off */
void LEDOneShotTimerCallback(TimerHandle_t xTimer);

/*-----------------------------------------------------------
 * HELPERS
 *----------------------------------------------------------*/

/**
 * Processing of the LoRa interrupt is handled by this task. */

void loraHandlerTask(void *pvParameters);

/**
 * Pretty print a packet to th e serial port.
 *
 * @param uiTimeStamp The timestamp of the thing printed
 * @param packetBuffer The buffer to print
 * @param bytesWritten The length of the buffer
 */
void vPrintPacket(uint16_t uiTimeStamp, uint8_t *packetBuffer, uint8_t bytesWritten);