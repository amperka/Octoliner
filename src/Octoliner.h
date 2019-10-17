/*
 * This file is a part of Octoliner library.
 *
 * Product page: https://amperka.ru/product/zelo-folow-line-sensor
 * © Amperka LLC (https://amperka.com, dev@amperka.com)
 * 
 * Author: Vasily Basalaev <vasily@amperka.ru>
 * Refactored by: Yury Botov <by@amperka.com>
 * License: GPLv3, all text here must be included in any redistribution.
 */

#include "GpioExpander.h"
#include <Arduino.h>
#include <Wire.h>

#ifndef __OCTOLINER_H__
#define __OCTOLINER_H__

class Octoliner : public GpioExpander {
public:
    Octoliner(uint8_t i2cAddress = 42);
    void begin();
    void begin(TwoWire* wire);
    void setSensitivity(uint8_t sense);
    int16_t analogRead(uint8_t sensor);
    void analogReadAll(int16_t* analogValues);
    uint8_t digitalReadAll(void);

    float trackLine(void);
    float trackLine(uint8_t pattern);
    float trackLine(int16_t* analogValues);

    virtual uint8_t mapAnalogToPattern(int16_t* analogValues) const;
    virtual float mapPatternToLine(uint8_t pattern) const;

    // Deprecated mapLine
    float mapLine(int* analogValues);

private:
    static constexpr uint8_t _IRLedsPin = 9;
    static constexpr uint8_t _sensePin = 0;
    static const uint8_t _sensorPinMap[8];
    float _previousValue;
};

#endif // __OCTOLINER_H__
