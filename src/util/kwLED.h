#pragma once

/**
 *  Pin handling made easy
 */

#include <Arduino.h>

class Pin
{
public:
    Pin() {}
    Pin(uint8_t pin) : _pin(pin) {}

    uint8_t pin() { return _pin; }
    uint16_t value() { return _value; }

protected:
    uint8_t _pin = {LED_BUILTIN};
    uint16_t _value = {0};
};

class kwLED : public Pin
{
public:
    kwLED() : Pin() {}
    kwLED(uint8_t pin) : Pin(pin) {}

    void begin() { pinMode(_pin, OUTPUT); }
    void activeLow() { _positive = LOW; }
    void turnOn() { write(_positive); }
    void turnOff() { write(!_positive); }
    void toggle() { _value == _positive ? turnOff() : turnOn(); }

    void blink(uint8_t times = 1, uint16_t interval = 100)
    {
        while (times-- > 0)
        {
            turnOn();
            delay(interval);
            turnOff();
            delay(interval);
        }
    }

protected:
    bool _positive = {HIGH};
    void write(bool value)
    {
        digitalWrite(_pin, value ? HIGH : LOW);
    }
};