/*
 * This file is a part of Dragster car set library.
 *
 * Defines: octoliner object - 8-point line-folowing sensor driver
 * © Amperka LLC (https://amperka.com, dev@amperka.com)
 * 
 * Author: Vasily Basalaev <vasily@amperka.ru>
 * Refactored by: Yury Botov <by@amperka.com>
 * License: GPLv3, all text here must be included in any redistribution.
 */

#include <Arduino.h>

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

// Pins definiion
constexpr uint8_t LED_BRIGHTNESS_PIN = 9;
constexpr uint8_t SENSITIVITY_PIN = 0;

enum IOcommand {
    WHO_AM_I //get UID
    ,
    RESET // reset
    ,
    CHANGE_I2C_ADDR // change I2C-address manually
    ,
    SAVE_I2C_ADDR // Save current address to flash
    ,
    PORT_MODE_INPUT // set pins to INPUT mode
    ,
    PORT_MODE_PULLUP // ... with pull-up internal resistor
    ,
    PORT_MODE_PULLDOWN // .. with pull-down internal resistor
    ,
    PORT_MODE_OUTPUT // set pins to OUTPUT mode
    ,
    DIGITAL_READ // read port (all of 8 pins)
    ,
    DIGITAL_WRITE_HIGH //
    ,
    DIGITAL_WRITE_LOW //
    ,
    ANALOG_WRITE //
    ,
    ANALOG_READ //
    ,
    PWM_FREQ // common pwm frequency to all pins
    ,
    ADC_SPEED,
    SEND_MASTER_READED_UID,
    CHANGE_I2C_ADDR_IF_UID_OK
};

class Octoliner {
public:
    Octoliner();
    Octoliner(uint8_t i2caddress);
    void begin(uint8_t value);
    void begin(void);
    void digitalWrite(int pin, bool value);
    void pinMode(int pin, uint8_t mode);
    void analogWrite(int pin, uint8_t pulseWidth);
    void pwmFreq(uint16_t freq);
    void changeAddr(uint8_t newAddr);
    void changeAddrWithUID(uint8_t newAddr);
    void saveAddr();
    void reset();
    int digitalRead(int pin);
    int analogRead(int pin);
    uint32_t getUID();
    int digitalReadPort();
    void digitalWritePort(uint16_t value);
    void pinModePort(uint16_t value, uint8_t mode);
    void adcSpeed(uint8_t speed);
    void setSensitivity(uint8_t sense);
    void setBrightness(uint8_t brightness);
    float mapLine(int binaryLine[8]);

private:
    uint8_t _i2caddress;
    float _value;
    void writeCmdPin(IOcommand command, uint8_t pin, bool sendStop = true);
    void writeCmdPin16Val(IOcommand command, uint8_t pin, uint16_t value, bool sendStop = true);
    void writeCmd16BitData(IOcommand command, uint16_t data);
    void writeCmd8BitData(IOcommand command, uint8_t data);
    void writeCmd(IOcommand command, bool sendStop = true);
    int read16Bit();
    uint32_t read32bit();
};

#endif //__OCTOLINER_H__
