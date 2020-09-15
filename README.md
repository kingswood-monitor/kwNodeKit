# kwNodeKit

**An Ardiuno library for rapidly building firmware for embedded sensor nodes.**

## Overview

`kwNodeKit` provides a library for wirelessly transmitting and receiving  information between embedded sensors and concentrators. A `node` is a single device with one or more `transports` and zero or more `sensors`. 

A sensor is a physical sensor e.g. a DHT22 temperature and humidity sensor, or a virtual sensor e.g. node battery voltage. A transport device is a wireless link e.g. LoRa.

A sensor node can be constructed from a transport and one or more sensors. A concentrator/bridge can be constructed from a LoRa and a wifi transport.

Transmission is efficient. Sensor data is encoded in Google's `protobuf` format. Transmission only occurs if data has changed (_'Report By Exception'_).

`kwNodeKit` comes with drivers for the following physical sensors:

* [Vishay Semiconductor VEML7700](https://www.vishay.com/docs/84286/veml7700.pdf) ambient light sensor
* [DHT22](https://www.adafruit.com/product/385) temperature and humidity sensor
* [Texas Instruments HDC1080](https://www.ti.com/product/HDC1080) temperature and humidity sensor

`kwNodeKit` provides drivers for the following virtual sensors:

* MCU Battery voltage (for MCUs with battery support)
* LoRa signal parameters RSSI, SNR, Frequency Error (for devices with LorA)
* Wifi signal parameters RSSI (for devices with WiFi)
* Dewpoint (for devices with temperature and relative humidity)

`kwNodeKit` provides the following transports:

* LoRa (RF95, SX1276)
* WiFi

New sensors and transports can be created by subclassing `kwSensor` and `kwTransport` and defining the relevant virtual methods. See the existing drivers for examples.

`kwNodeKit` has been tested on the following devices:

* Adafruit 32u4 Feather RF95 'LoRa'
* TTGO ESP32 SX1276 LoRa

## Protobuf specification
The protobuf specification can be modified and recompiled to suit your own needs. See the `readme.md` in `src/protobuf-packet/` for more information.

The code is provided as-is and without warranty under the MIT licence. I can be contacted at `richlyon@mac.com`.

## Change log

### 19.05.20 v0.1.0
- Initial release

### 15.09.20 v0.2.0
- Code documentation :)
- Added Interrupt Service Routine and handler based on FreeRTOS API