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
#include <Wire.h>

void Octoliner::begin(uint8_t value) {
    Wire.begin();
    pwmFreq(30000);
    setSensitivity(value);
    Octoliner::pinMode(_brightnessPin, OUTPUT);
    Octoliner::digitalWrite(_brightnessPin, HIGH);
    _lastPosition = 0;
}

void Octoliner::writeCmdPin(IOcommand command, uint8_t pin, bool sendStop) {
    Wire.beginTransmission(_i2caddress);
    Wire.write((uint8_t)command);
    Wire.write(pin);
    Wire.endTransmission(sendStop);
}

void Octoliner::writeCmdPin16Val(IOcommand command, uint8_t pin, uint16_t value, bool sendStop) {
    Wire.beginTransmission(_i2caddress);
    Wire.write((uint8_t)command);
    Wire.write(pin);
    uint8_t temp;
    temp = (value >> 8) & 0xff;
    Wire.write(temp); // Data/setting to be sent to device
    temp = value & 0xff;
    Wire.write(temp); // Data/setting to be sent to device
    Wire.endTransmission(sendStop);
}

void Octoliner::writeCmd16BitData(IOcommand command, uint16_t data) {
    Wire.beginTransmission(_i2caddress); // Address set on class instantiation
    Wire.write((uint8_t)command);
    uint8_t temp;
    temp = (data >> 8) & 0xff;
    Wire.write(temp); // Data/setting to be sent to device
    temp = data & 0xff;
    Wire.write(temp); // Data/setting to be sent to device
    Wire.endTransmission();
}

void Octoliner::writeCmd8BitData(IOcommand command, uint8_t data) {
    Wire.beginTransmission(_i2caddress); // Address set on class instantiation
    Wire.write((uint8_t)command);
    Wire.write(data); // Data/setting to be sent to device
    Wire.endTransmission();
}

void Octoliner::writeCmd(IOcommand command, bool sendStop) {
    Wire.beginTransmission(_i2caddress);
    Wire.write((uint8_t)command);
    Wire.endTransmission(sendStop);
}

uint16_t Octoliner::read16Bit() {
    uint16_t result = 0xffff;
    uint8_t byteCount = 2;
    Wire.requestFrom(_i2caddress, byteCount);
    uint16_t counter = 0xffff;
    while (Wire.available() < byteCount) {
        if (!(--counter))
            return result;
    }
    result = 0;
    result = Wire.read();
    result <<= 8;
    result |= Wire.read();
    return result;
}

uint32_t Octoliner::read32bit() {
    uint32_t result = 0xffffffff; // https://www.youtube.com/watch?v=y73hyMP1a-E
    uint8_t byteCount = 4;
    Wire.requestFrom(_i2caddress, byteCount);
    uint16_t counter = 0xffff;

    while (Wire.available() < byteCount) {
        if (!(--counter))
            return result;
    }

    result = 0;
    for (uint8_t i = 0; i < byteCount - 1; ++i) {
        result |= Wire.read();
        result <<= 8;
    }
    result |= Wire.read();
    return result;
}

Octoliner::Octoliner(uint8_t i2caddress) {
    _i2caddress = i2caddress;
}

void Octoliner::digitalWritePort(uint16_t value) {
    writeCmd16BitData(DIGITAL_WRITE_HIGH, value);
    writeCmd16BitData(DIGITAL_WRITE_LOW, ~value);
}

void Octoliner::digitalWrite(uint8_t pin, bool value) {
    uint16_t sendData = 1 << pin;
    if (value) {
        writeCmd16BitData(DIGITAL_WRITE_HIGH, sendData);
    } else {
        writeCmd16BitData(DIGITAL_WRITE_LOW, sendData);
    }
}

uint8_t Octoliner::digitalReadPort() {
    writeCmd(DIGITAL_READ, false);
    uint16_t readPort = read16Bit();
    uint8_t result = 0;
    result |= (readPort >> 1) & 0b00000001;
    result |= (readPort >> 1) & 0b00000010;
    result |= (readPort >> 1) & 0b00000100;
    result |= (readPort >> 5) & 0b00001000;
    result |= (readPort >> 3) & 0b00010000;
    result |= (readPort >> 1) & 0b00100000;
    result |= (readPort << 2) & 0b01000000;
    result |= (readPort << 2) & 0b10000000;
    // инвертировать порядок битов
    return result;
}

uint8_t Octoliner::digitalRead(uint8_t pin) {
    return ((digitalReadPort() & (1 << pin)) ? 1 : 0);
}

float Octoliner::mapLine(int16_t binaryLine[8]) {
    uint8_t pattern = 0;

    // search min and max values
    int16_t min = 32767;
    int16_t max = 0;
    for (uint8_t i = 0; i < 8; i++) {
        if (binaryLine[i] < min)
            min = binaryLine[i];
        if (binaryLine[i] > max)
            max = binaryLine[i];
    }
    // calculate threshold level
    int16_t threshold = min + (max - min) / 2;
    // create bit pattern
    for (uint8_t i = 0; i < 8; i++) {
        pattern = (pattern << 1) + ((binaryLine[i] < threshold) ? 0 : 1);
    }
    // interpret pattern
    float position;
    switch (pattern) {
    case 0b00011000:
        position = 0;
        break;
    case 0b00010000:
        position = 0.25;
        break;
    case 0b00111000:
        position = 0.25;
        break;
    case 0b00001000:
        position = -0.25;
        break;
    case 0b00011100:
        position = -0.25;
        break;
    case 0b00110000:
        position = 0.375;
        break;
    case 0b00001100:
        position = -0.375;
        break;
    case 0b00100000:
        position = 0.5;
        break;
    case 0b01110000:
        position = 0.5;
        break;
    case 0b00000100:
        position = -0.5;
        break;
    case 0b00001110:
        position = -0.5;
        break;
    case 0b01100000:
        position = 0.625;
        break;
    case 0b11100000:
        position = 0.625;
        break;
    case 0b00000110:
        position = -0.625;
        break;
    case 0b00000111:
        position = -0.625;
        break;
    case 0b01000000:
        position = 0.75;
        break;
    case 0b11110000:
        position = 0.75;
        break;
    case 0b00000010:
        position = -0.75;
        break;
    case 0b00001111:
        position = -0.75;
        break;
    case 0b11000000:
        position = 0.875;
        break;
    case 0b00000011:
        position = -0.875;
        break;
    case 0b10000000:
        position = 1.0;
        break;
    case 0b00000001:
        position = -1.0;
        break;
    default:
        position = _lastPosition;
        break;
    }
    _lastPosition = position;
    return position;
}

void Octoliner::pinModePort(uint16_t value, uint8_t mode) {
    if (mode == INPUT) {
        writeCmd16BitData(PORT_MODE_INPUT, value);
    } else if (mode == OUTPUT) {
        writeCmd16BitData(PORT_MODE_OUTPUT, value);
    } else if (mode == INPUT_PULLUP) {
        writeCmd16BitData(PORT_MODE_PULLUP, value);
    } else if (mode == INPUT_PULLDOWN) {
        writeCmd16BitData(PORT_MODE_PULLDOWN, value);
    }
}

void Octoliner::pinMode(uint8_t pin, uint8_t mode) {
    uint16_t sendData = 1 << pin;
    pinModePort(sendData, mode);
}

void Octoliner::analogWrite(uint8_t pin, uint8_t pulseWidth) {
    uint16_t val = map(pulseWidth, 0, 255, 0, 65535);
    writeCmdPin16Val(ANALOG_WRITE, pin, val, true);
}

int16_t Octoliner::analogRead(uint8_t pin) {
    uint8_t mapper[8] = { 4, 5, 6, 8, 7, 3, 2, 1 };
    writeCmdPin(ANALOG_READ, mapper[pin], true);
    return read16Bit();
}

void Octoliner::pwmFreq(uint16_t freq) {
    writeCmd16BitData(PWM_FREQ, freq);
}

void Octoliner::adcSpeed(uint8_t speed) {
    // speed must be < 8. Smaller is faster, but dirty
    writeCmd8BitData(ADC_SPEED, speed);
}

void Octoliner::changeAddr(uint8_t newAddr) {
    writeCmd8BitData(CHANGE_I2C_ADDR, newAddr);
    _i2caddress = newAddr;
}

void Octoliner::changeAddrWithUID(uint8_t newAddr) {
    uint32_t uid = getUID();

    delay(1);

    Wire.beginTransmission(_i2caddress); // Address set on class instantiation

    Wire.write((uint8_t)SEND_MASTER_READED_UID);
    uint8_t temp;
    temp = (uid >> 24) & 0xff;
    Wire.write(temp); // Data/setting to be sent to device

    temp = (uid >> 16) & 0xff;
    Wire.write(temp); // Data/setting to be sent to device

    temp = (uid >> 8) & 0xff;
    Wire.write(temp); // Data/setting to be sent to device

    temp = (uid)&0xff;
    Wire.write(temp); // Data/setting to be sent to device
    Wire.endTransmission();

    delay(1);

    writeCmd8BitData(CHANGE_I2C_ADDR_IF_UID_OK, newAddr);
    _i2caddress = newAddr;

    delay(1);
}

void Octoliner::saveAddr() {
    writeCmd(SAVE_I2C_ADDR);
}

void Octoliner::reset() {
    writeCmd(RESET);
}

uint32_t Octoliner::getUID() {
    writeCmd(WHO_AM_I);
    return read32bit();
}

void Octoliner::setSensitivity(uint8_t sensitivity) {
    Octoliner::analogWrite(_sensitivityPin, sensitivity);
}

void Octoliner::setBrightness(uint8_t brightness) {
    (void)brightness;
}
