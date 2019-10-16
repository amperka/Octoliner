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

#include "Octoliner.h"

Octoliner::Octoliner(uint8_t i2caddress)
    : GpioExpander(i2caddress) {
    _previousValue = 0.;
}

void Octoliner::begin() {
    Octoliner::begin(&Wire);
}

void Octoliner::begin(TwoWire* wire) {
    wire->begin();
    GpioExpander::begin(wire);
    GpioExpander::pwmFreq(8000); // ~ 250 different pwm values
    GpioExpander::pinMode(_IRLedsPin, OUTPUT);
    GpioExpander::digitalWrite(_IRLedsPin, HIGH);
}

void Octoliner::setSensitivity(uint8_t sense) {
    analogWrite(_sensePin, sense);
}

int16_t Octoliner::analogRead(uint8_t sensor) {
    return (int16_t)GpioExpander::analogRead(_sensorPinMap[sensor & 0x07]);
}

void Octoliner::analogReadAll(int16_t* analogValues) {
    for (uint8_t i = 0; i < 8; i++) {
        analogValues[i] = Octoliner::analogRead(i);
    }
}

uint8_t Octoliner::mapAnalogToPattern(int16_t* analogValues) const {
    uint8_t pattern = 0;

    // search min and max values
    int16_t min = 32767;
    int16_t max = 0;
    for (uint8_t i = 0; i < 8; i++) {
        if (analogValues[i] < min)
            min = analogValues[i];
        if (analogValues[i] > max)
            max = analogValues[i];
    }
    // calculate threshold level
    int16_t threshold = min + (max - min) / 2;
    // create bit pattern
    for (uint8_t i = 0; i < 8; i++) {
        pattern = (pattern << 1) + ((analogValues[i] < threshold) ? 0 : 1);
    }
    return pattern;
}

float Octoliner::mapPatternToLine(uint8_t binaryLine) const {
    switch (binaryLine) {
    case 0b00011000: return 0;
    case 0b00010000: return 0.25;
    case 0b00111000: return 0.25;
    case 0b00001000: return -0.25;
    case 0b00011100: return -0.25;
    case 0b00110000: return 0.375;
    case 0b00001100: return -0.375;
    case 0b00100000: return 0.5;
    case 0b01110000: return 0.5;
    case 0b00000100: return -0.5;
    case 0b00001110: return -0.5;
    case 0b01100000: return 0.625;
    case 0b11100000: return 0.625;
    case 0b00000110: return -0.625;
    case 0b00000111: return -0.625;
    case 0b01000000: return 0.75;
    case 0b11110000: return 0.75;
    case 0b00000010: return -0.75;
    case 0b00001111: return -0.75;
    case 0b11000000: return 0.875;
    case 0b00000011: return -0.875;
    case 0b10000000: return 1.0;
    case 0b00000001: return -1.0;
    default: return NAN;
    }
    return NAN;
}

float Octoliner::trackLine(void) {
    int16_t analogValues[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
    analogReadAll(analogValues);
    return trackLine(analogValues);
}

float Octoliner::trackLine(uint8_t pattern) {
    float result = mapPatternToLine(pattern);
    result = isnan(result) ? _previousValue : result;
    _previousValue = result;
    return result;
}

float Octoliner::trackLine(int16_t* analogValues) {
    return trackLine(mapAnalogToPattern(analogValues));
}

uint8_t Octoliner::digitalReadAll(void) {
    int16_t analogValues[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
    analogReadAll(analogValues);
    return mapAnalogToPattern(analogValues);
}

/*---------  Deprecated ----------*/

float Octoliner::mapLine(int* analogValues) {
    return trackLine(analogValues);
}
