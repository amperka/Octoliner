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
#include <Wire.h>

GpioExpander::GpioExpander(uint8_t i2cAddress) {
    _i2cAddress = i2cAddress;
}

void GpioExpander::begin(void) {
    begin(&Wire);
}

void GpioExpander::begin(TwoWire* wire) {
    _wire = wire;
}

void GpioExpander::reset(void) {
    writeCmd(RESET_SLAVE);
}

void GpioExpander::pinMode(uint8_t pin, uint8_t mode) {
    pinModePort(1 << pin, mode);
}

void GpioExpander::digitalWrite(uint8_t pin, bool value) {
    uint16_t sendData = 1 << pin;
    if (value) {
        writeCmd16BitData(DIGITAL_WRITE_HIGH, sendData);
    } else {
        writeCmd16BitData(DIGITAL_WRITE_LOW, sendData);
    }
}

uint16_t GpioExpander::mapResolution(uint16_t value, uint8_t from, uint8_t to) {
    if (from == to)
        return value;
    if (from > to)
        return value >> (from - to);
    else
        return value << (to - from);
}

void GpioExpander::analogWriteResolution(uint8_t res) {
    _analogWriteResolution = res;
}

void GpioExpander::analogReadResolution(uint8_t res) {
    _analogReadResolution = res;
}

void GpioExpander::analogWrite(uint8_t pin, uint16_t pulseWidth) {
    uint16_t val = mapResolution(pulseWidth, _analogWriteResolution, 16);
    writeCmdPin16Val(ANALOG_WRITE, pin, val, true);
}

int16_t GpioExpander::digitalRead(uint8_t pin) {
    int16_t portValue = digitalReadPort();
    if (portValue >= 0)
        return (portValue & (1 << pin) ? 1 : 0);
    else
        return 0;
}

int16_t GpioExpander::analogRead(uint8_t pin) {
    writeCmdPin(ANALOG_READ, pin, true);
    int16_t result = read16Bit();
    if (result >= 0) {
        result = (int16_t)mapResolution((uint16_t)result, 12, _analogReadResolution);
    }
    return result;
}

void GpioExpander::changeAddr(uint8_t newAddr) {
    writeCmd8BitData(CHANGE_I2C_ADDR, newAddr);
    _i2cAddress = newAddr;
}

void GpioExpander::changeAddrWithUID(uint8_t newAddr) {
    uint32_t uid = getUID();

    delay(1);
    // address set on class instantiation
    _wire->beginTransmission(_i2cAddress);

    _wire->write((uint8_t)MASTER_READED_UID);
    // upper byte of data/setting to be sent to device
    _wire->write((uid >> 24) & 0xff);
    // third byte of data/setting to be sent to device
    _wire->write((uid >> 16) & 0xff);
    // second byte of data/setting to be sent to device
    _wire->write((uid >> 8) & 0xff);
    // lower byte of data/setting to be sent to device
    _wire->write(uid & 0xff);
    _wire->endTransmission();

    delay(1);

    writeCmd8BitData(CHANGE_I2C_ADDR_IF_UID_OK, newAddr);
    _i2cAddress = newAddr;

    delay(1);
}

void GpioExpander::saveAddr() {
    writeCmd(SAVE_I2C_ADDR);
}

void GpioExpander::pinModePort(uint16_t value, uint8_t mode) {
    switch (mode) {
    case INPUT:
        writeCmd16BitData(PORT_MODE_INPUT, value);
        break;
    case OUTPUT:
        writeCmd16BitData(PORT_MODE_OUTPUT, value);
        break;
    case INPUT_PULLUP:
        writeCmd16BitData(PORT_MODE_PULLUP, value);
        break;
    case INPUT_PULLDOWN:
        writeCmd16BitData(PORT_MODE_PULLDOWN, value);
        break;
    default:
        break;
    }
}

void GpioExpander::digitalWritePort(uint16_t value) {
    writeCmd16BitData(DIGITAL_WRITE_HIGH, value);
    writeCmd16BitData(DIGITAL_WRITE_LOW, ~value);
}

int16_t GpioExpander::digitalReadPort() {
    // check. Need it? maybe true?
    writeCmd(DIGITAL_READ, false);
    return read16Bit();
}

void GpioExpander::pwmFreq(uint16_t freq) {
    writeCmd16BitData(PWM_FREQ, freq);
}

uint32_t GpioExpander::getUID() {
    writeCmd(UID);
    return read32Bit();
}

void GpioExpander::adcSpeed(uint8_t speed) {
    // speed must be < 8. Smaller is faster, but dirty
    writeCmd8BitData(ADC_SPEED, speed & 0x07);
}

void GpioExpander::adcFilter(bool enable) {
    IOcommand command = enable ? ADC_LOWPASS_FILTER_ON : ADC_LOWPASS_FILTER_OFF;
    writeCmd(command);
}

void GpioExpander::setEncoderPins(uint8_t encoder, uint8_t pinA, uint8_t pinB) {
    uint8_t pins = (pinA << 4) | (pinB & 0x0f);
    uint16_t payload = ((uint16_t)encoder << 8) | pins;
    writeCmd16BitData(ENCODER_SET_PINS, payload);
}

int8_t GpioExpander::readEncoderDiff(uint8_t encoder) {
    writeCmdPin(ENCODER_GET_DIFF_VALUE, encoder, true);
    return readInt8Bit();
}

void GpioExpander::writeCmdPin(IOcommand command, uint8_t pin, bool sendStop) {
    _wire->beginTransmission(_i2cAddress);
    _wire->write((uint8_t)command);
    _wire->write(pin);
    _wire->endTransmission(sendStop);
}

void GpioExpander::writeCmdPin16Val(IOcommand command, uint8_t pin, uint16_t value, bool sendStop) {
    _wire->beginTransmission(_i2cAddress);
    _wire->write((uint8_t)command);
    _wire->write(pin);
    // upper byte of data/setting to be sent to device
    _wire->write((value >> 8) & 0xff);
    // lower byte of data/setting to be sent to device
    _wire->write(value & 0xff);
    _wire->endTransmission(sendStop);
}

void GpioExpander::writeCmd16BitData(IOcommand command, uint16_t data) {
    // address set on class instantiation
    _wire->beginTransmission(_i2cAddress);
    _wire->write((uint8_t)command);
    // upper byte of data/setting to be sent to device
    _wire->write((data >> 8) & 0xff);
    // lower byte of data/setting to be sent to device
    _wire->write(data & 0xff);
    _wire->endTransmission();
}

void GpioExpander::writeCmd8BitData(IOcommand command, uint8_t data) {
    // address set on class instantiation
    _wire->beginTransmission(_i2cAddress);
    _wire->write((uint8_t)command);
    // data/setting to be sent to device
    _wire->write(data);
    _wire->endTransmission();
}

void GpioExpander::writeCmd(IOcommand command, bool sendStop) {
    _wire->beginTransmission(_i2cAddress);
    _wire->write((uint8_t)command);
    _wire->endTransmission(sendStop);
}

int8_t GpioExpander::readInt8Bit() {
    const uint8_t byteCount = 1;
    _wire->requestFrom(_i2cAddress, byteCount);
    if (_wire->available() != byteCount)
        return -1;
    else
        return (int8_t)_wire->read();
}

int16_t GpioExpander::read16Bit() {
    int16_t result = -1;
    const uint8_t byteCount = 2;
    _wire->requestFrom(_i2cAddress, byteCount);
    if (_wire->available() != byteCount)
        return result;
    result = _wire->read();
    result = (result << 8) | _wire->read();
    return result;
}

uint32_t GpioExpander::read32Bit() {
    uint32_t result = 0;
    const uint8_t byteCount = 4;
    _wire->requestFrom(_i2cAddress, byteCount);
    if (_wire->available() != byteCount)
        return 0xffffffff;
    for (uint8_t i = 0; i < byteCount; i++) {
        result = (result << 8) | _wire->read();
    }
    return result;
}
