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

#include <arduino.h>

#ifndef __OCTOLINER_H__
#define __OCTOLINER_H__

#ifndef INPUT_PULLDOWN
#define INPUT_PULLDOWN 0x3
#endif
//INPUT_PULLDOWN

#ifndef DEFAULT_GPIOEXP_ADDR
#define DEFAULT_GPIOEXP_ADDR 42
#endif
// DEFAULT_GPIOEXP_ADDR

enum IOcommand {
    WHO_AM_I, //get UID
    RESET,
    CHANGE_I2C_ADDR, // change I2C-address manually
    SAVE_I2C_ADDR, // Save current address to flash
    PORT_MODE_INPUT, // set pins to INPUT mode
    PORT_MODE_PULLUP, // ... with pull-up internal resistor
    PORT_MODE_PULLDOWN, // .. with pull-down internal resistor
    PORT_MODE_OUTPUT, // set pins to OUTPUT mode
    DIGITAL_READ, // read port (all of 8 pins)
    DIGITAL_WRITE_HIGH,
    DIGITAL_WRITE_LOW,
    ANALOG_WRITE,
    ANALOG_READ,
    PWM_FREQ, // common pwm frequency to all pins
    ADC_SPEED,
    SEND_MASTER_READED_UID,
    CHANGE_I2C_ADDR_IF_UID_OK
};

class Octoliner {
public:
    Octoliner(uint8_t i2caddress = 42);
    void begin(uint8_t value = 208);
    void changeAddr(uint8_t newAddr);
    void changeAddrWithUID(uint8_t newAddr);
    void saveAddr();
    void reset();
    int16_t analogRead(uint8_t pin);
    uint32_t getUID();
    void adcSpeed(uint8_t speed); // speed must be < 8. Smaller is faster, but dirty
    float mapLine(int16_t binaryLine[8]);
    void setSensitivity(uint8_t sensitivity);
    void setBrightness(uint8_t brightness); // dummy

private:
    static constexpr uint8_t _sensitivityPin = 0;
    static constexpr uint8_t _brightnessPin = 9;

    uint8_t _i2caddress;
    float _lastPosition;
    void writeCmdPin(IOcommand command, uint8_t pin, bool sendStop = true);
    void writeCmdPin16Val(IOcommand command, uint8_t pin, uint16_t value, bool sendStop = true);
    void writeCmd16BitData(IOcommand command, uint16_t data);
    void writeCmd8BitData(IOcommand command, uint8_t data);
    void writeCmd(IOcommand command, bool sendStop = true);
    uint16_t read16Bit();
    uint32_t read32bit();

    void digitalWrite(uint8_t pin, bool value);
    void pinMode(uint8_t pin, uint8_t mode);
    void analogWrite(uint8_t pin, uint8_t pulseWidth);
    void pwmFreq(uint16_t freq);
    void pinModePort(uint16_t value, uint8_t mode);
};

#endif //__OCTOLINER_H__
