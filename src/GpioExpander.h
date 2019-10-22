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

#include <Arduino.h>
#include <Wire.h>
#include "i2cioCommands.h"

#ifndef __GPIOEXPANDER_H__
#define __GPIOEXPANDER_H__

#ifndef INPUT_PULLDOWN
#define INPUT_PULLDOWN 0x3
#endif

#ifndef DEFAULT_GPIOEXP_ADDR
#define DEFAULT_GPIOEXP_ADDR 42
#endif

#ifndef I2CIO_PIN_COUNT
#define I2CIO_PIN_COUNT 10
#endif

// we need to say something to identity... Let's say "slot"
#define IS_HE_SAY_SLOT (uint32_t)(((uint32_t)'s' << 24) | ((uint32_t)'l' << 16) | ((uint32_t)'o' << 8) | ((uint32_t)'t'))

class GpioExpander {
public:
    GpioExpander(uint8_t i2cAddress = DEFAULT_GPIOEXP_ADDR);
    void begin(void);
    void begin(TwoWire* wire);
    void reset(void);
    void pinMode(uint8_t pin, uint8_t mode);
    void analogReadResolution(uint8_t res);
    void analogWriteResolution(uint8_t res);
    void digitalWrite(uint8_t pin, bool value);
    void analogWrite(uint8_t pin, uint16_t pulseWidth);
    int16_t digitalRead(uint8_t pin);
    int16_t analogRead(uint8_t pin);
    void changeAddr(uint8_t newAddr);
    void changeAddrWithUID(uint8_t newAddr);
    void saveAddr(void);
    void pinModePort(uint16_t value, uint8_t mode);
    void digitalWritePort(uint16_t value);
    int16_t digitalReadPort(void);
    void pwmFreq(uint16_t freq);
    uint32_t getUID(void);
    void adcSpeed(uint8_t speed);
    void adcFilter(bool enable);
    void setEncoderPins(uint8_t encoder, uint8_t pinA, uint8_t pinB);
    int8_t readEncoderDiff(uint8_t encoder);

private:
    TwoWire* _wire;
    uint8_t _i2cAddress;
    uint8_t _analogWriteResolution = 8;
    uint8_t _analogReadResolution = 10;
    void writeCmdPin(IOcommand command, uint8_t pin, bool sendStop = true);
    void writeCmdPin16Val(IOcommand command, uint8_t pin, uint16_t value, bool sendStop = true);
    void writeCmd16BitData(IOcommand command, uint16_t data);
    void writeCmd8BitData(IOcommand command, uint8_t data);
    void writeCmd(IOcommand command, bool sendStop = true);
    int8_t readInt8Bit(void);
    int16_t read16Bit(void); // Name change to readInt16Bit?
    uint32_t read32Bit(void);
    uint16_t mapResolution(uint16_t value, uint8_t from, uint8_t to);
};

#endif //__GPIOEXPANDER_H__
