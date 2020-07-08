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

// Minimum adequate sensitivity. For all values below this, channels always
// see black even on mirror/white surfaces
constexpr uint8_t MIN_SENSITIVITY = 120;
// All analog values above this are considered black
constexpr int16_t BLACK_THRESHOLD = 100;

class Octoliner : private GpioExpander {
public:
    Octoliner(uint8_t i2cAddress = 42);
    void begin();
    void begin(TwoWire* wire);
    void analogReadResolution(uint8_t resolution);
    int16_t analogRead(uint8_t sensor);
    void analogReadAll(int16_t* analogValues);
    uint8_t digitalReadAll();

    float trackLine();
    float trackLine(uint8_t pattern);
    float trackLine(int16_t* analogValues);

    virtual uint8_t mapAnalogToPattern(int16_t* analogValues) const;
    virtual float mapPatternToLine(uint8_t pattern) const;

    bool optimizeSensitivityOnBlack();
    void setSensitivity(uint8_t sense);
    uint8_t getSensitivity() const;

    void changeAddr(uint8_t newAddr);
    void saveAddr();

    // Deprecated mapLine
    float mapLine(int* analogValues);

private:
    uint8_t countOfBlack();
    static constexpr uint8_t _IRLedsPin = 9;
    static constexpr uint8_t _sensePin = 0;
    static const uint8_t _sensorPinMap[8];
    uint8_t _sensitivity;
    float _previousValue;
};

#endif // __OCTOLINER_H__
