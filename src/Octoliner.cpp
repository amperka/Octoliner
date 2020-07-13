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

const uint8_t Octoliner::_sensorPinMap[] = {
    4, 5, 6, 8, 7, 3, 2, 1
};

Octoliner::Octoliner(uint8_t i2cAddress)
    : GpioExpander(i2cAddress) {
    _previousValue = 0.;
    _sensitivity = 208;
}

void Octoliner::begin() {
    Octoliner::begin(&Wire);
}

void Octoliner::begin(TwoWire* wire) {
    wire->begin();
    GpioExpander::begin(wire);
    GpioExpander::pwmFreq(8000); // ~ 250 pwm levels
    GpioExpander::pinMode(_IRLedsPin, OUTPUT);
    GpioExpander::digitalWrite(_IRLedsPin, HIGH);
    GpioExpander::analogReadResolution(10);
}

void Octoliner::setSensitivity(uint8_t sense) {
    _sensitivity = sense;
    analogWrite(_sensePin, _sensitivity);
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

float Octoliner::trackLine() {
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

uint8_t Octoliner::digitalReadAll() {
    int16_t analogValues[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
    analogReadAll(analogValues);
    return mapAnalogToPattern(analogValues);
}

uint8_t Octoliner::countOfBlack() {
    uint8_t count = 0;
    for (uint8_t i = 0; i < 8; i++) {
        if (analogRead(i) > BLACK_THRESHOLD)
            count++;
    }
    return count;
}

bool Octoliner::optimizeSensitivityOnBlack() {
    uint8_t sens;
    uint8_t sensitivityBackup = getSensitivity();
    // Give more time to settle RCL circuit at first time
    setSensitivity(255);
    delay(200);
    // Starting at the highest possible sensitivity read all channels at each iteration
    // to find the level when all the channels become black
    for (sens = 255; sens > MIN_SENSITIVITY; sens -= 5) {
        setSensitivity(sens);
        // Give some time to settle RCL circuit
        delay(100);
        if (countOfBlack() == 8)
            break;
    }
    if (sens <= MIN_SENSITIVITY) { // Somthing is broken
        setSensitivity(sensitivityBackup);
        return false;
    }

    // Forward fine search to find the level when at least one sensor value will
    // become back white
    for (/*sens = sens*/; sens < 255; sens++) {
        setSensitivity(sens);
        delay(50);
        if (countOfBlack() != 8)
            break;
    }
    if (sens == 255) {
        // Environment has changed since the start of the process
        setSensitivity(sensitivityBackup);
        return false;
    }
    // Magic 5 step back to fall back to all-eight-black
    sens -= 5;
    setSensitivity(sens);
    return true;
}

uint8_t Octoliner::getSensitivity() const {
    return _sensitivity;
}

void Octoliner::changeAddr(uint8_t newAddr) {
    GpioExpander::changeAddr(newAddr);
}

void Octoliner::saveAddr(void) {
    GpioExpander::saveAddr();
}

/*---------  Deprecated ----------*/

float Octoliner::mapLine(int* analogValues) {
    return trackLine((int16_t*)analogValues);
}

void Octoliner::begin(int sense) {
    _sensitivity = sense;
    Octoliner::begin(&Wire);
    GpioExpander::analogReadResolution(12);
}
