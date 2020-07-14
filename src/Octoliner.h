#include <arduino.h>

#ifndef _GPIO_EXPANDER_
#define _GPIO_EXPANDER_

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
    RESET, // reset
    CHANGE_I2C_ADDR, // change I2C-address manually
    SAVE_I2C_ADDR, // Save current address to flash
    PORT_MODE_INPUT, // set pins to INPUT mode
    PORT_MODE_PULLUP, // ... with pull-up internal resistor
    PORT_MODE_PULLDOWN, // .. with pull-down internal resistor
    PORT_MODE_OUTPUT, // set pins to OUTPUT mode
    DIGITAL_READ, // read port (all of 8 pins)
    DIGITAL_WRITE_HIGH, //
    DIGITAL_WRITE_LOW, //
    ANALOG_WRITE, //
    ANALOG_READ, //
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
    int analogRead(int pin); //, uint8_t avgCount = 2);
    uint32_t getUID();
    void adcSpeed(uint8_t speed);
    float mapLine(int16_t binaryLine[8]);
    void setSensitivity(uint8_t sensitivity);
    void setBrightness(uint8_t brightness); // dummy

private:
    static constexpr uint8_t _sensitivityPin = 0;
    static constexpr uint8_t _brightnessPin = 9;

    uint8_t _i2caddress;
    void writeCmdPin(IOcommand command, uint8_t pin, bool sendStop = true);
    void writeCmdPin16Val(IOcommand command, uint8_t pin, uint16_t value, bool sendStop = true);
    void writeCmd16BitData(IOcommand command, uint16_t data);
    void writeCmd8BitData(IOcommand command, uint8_t data);
    void writeCmd(IOcommand command, bool sendStop = true);
    int read16Bit();
    uint32_t read32bit();

    void digitalWrite(int pin, bool value);
    void pinMode(int pin, uint8_t mode);
    void analogWrite(int pin, uint8_t pulseWidth);
    void pwmFreq(uint16_t freq);
    int digitalRead(int pin);
    int digitalReadPort();
    void digitalWritePort(uint16_t value);
    void pinModePort(uint16_t value, uint8_t mode);    
};

#endif //_GPIO_EXPANDER_
