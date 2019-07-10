#include "Octoliner.h"
#include <Wire.h>

void Octoliner::begin(uint8_t brightness) {
    Wire.begin();
    pwmFreq(30000);
    analogWrite(0, brightness);
    value = 0;
}

void Octoliner::begin(void) {
    Wire.begin();
    pwmFreq(30000);
    analogWrite(0, 200);
    value = 0;
}

void Octoliner::writeCmdPin(IOcommand command, uint8_t pin, bool sendStop) {
    Wire.beginTransmission(_i2caddress);
    Wire.write((uint8_t)command);
    Wire.write(pin);
    Wire.endTransmission(sendStop);
}

void Octoliner::writeCmdPin16Val(IOcommand command, uint8_t pin, uint16_t data, bool sendStop) {
    Wire.beginTransmission(_i2caddress);
    Wire.write((uint8_t)command);
    Wire.write(pin);
    uint8_t temp;
    temp = (data >> 8) & 0xff;
    Wire.write(temp); // Send MSB of 'data' to the device
    temp = data & 0xff;
    Wire.write(temp); // Send LSB of 'data' to the device
    Wire.endTransmission(sendStop);
}

void Octoliner::writeCmd16BitData(IOcommand command, uint16_t data) {
    Wire.beginTransmission(_i2caddress); // Address set on class instantiation
    Wire.write((uint8_t)command);
    uint8_t temp;
    temp = (data >> 8) & 0xff;
    Wire.write(temp); // Send MSB of 'data' to the device
    temp = data & 0xff;
    Wire.write(temp); // Send LSB of 'data' to the device
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

int Octoliner::read16Bit() {
    int result = -1;
    uint8_t byteCount = 2;
    Wire.requestFrom(_i2caddress, byteCount);
    uint16_t counter = 0xffff;
    while (Wire.available() < byteCount) {
        if (!(--counter))
            return result;
    }
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

Octoliner::Octoliner() {
    _i2caddress = 42;
}

Octoliner::Octoliner(uint8_t i2caddress) {
    _i2caddress = i2caddress;
}

void Octoliner::digitalWritePort(uint16_t data) {
    writeCmd16BitData(DIGITAL_WRITE_HIGH, data);
    writeCmd16BitData(DIGITAL_WRITE_LOW, ~data);
}

void Octoliner::digitalWrite(int pin, bool state) {
    uint16_t sendData = 1 << pin;
    if (state) {
        writeCmd16BitData(DIGITAL_WRITE_HIGH, sendData);
    } else {
        writeCmd16BitData(DIGITAL_WRITE_LOW, sendData);
    }
}

int Octoliner::digitalReadPort() {
    writeCmd(DIGITAL_READ, false);
    int readPort = read16Bit();
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

int Octoliner::digitalRead(int pin) {
    return (digitalReadPort() >> pin) & 1;
}

float Octoliner::mapLine(int binaryLine[8]) {
    byte pattern = 0;
    // search min and max values
    int min = 32767;
    int max = 0;
    for (int i = 0; i < 8; i++) {
        if (binaryLine[i] < min)
            min = binaryLine[i];
        if (binaryLine[i] > max)
            max = binaryLine[i];
    }
    // calculate border level
    int threshold = min + (max - min) / 2;
    // create bit pattern
    for (int i = 0; i < 8; i++) {
        pattern = (pattern << 1) + ((binaryLine[i] < threshold) ? 0 : 1);
    }
    switch (pattern) {
    case 0b00011000:
        value = 0;
        break;
    case 0b00010000:
        value = 0.25;
        break;
    case 0b00001000:
        value = -0.25;
        break;
    case 0b00110000:
        value = 0.375;
        break;
    case 0b00001100:
        value = -0.375;
        break;
    case 0b00100000:
        value = 0.5;
        break;
    case 0b00000100:
        value = -0.5;
        break;
    case 0b01100000:
        value = 0.625;
        break;
    case 0b00000110:
        value = -0.625;
        break;
    case 0b01000000:
        value = 0.75;
        break;
    case 0b00000010:
        value = -0.75;
        break;
    case 0b11000000:
        value = 0.875;
        break;
    case 0b00000011:
        value = -0.875;
        break;
    case 0b10000000:
        value = 1.0;
        break;
    case 0b00000001:
        value = -1.0;
        break;
    default:
        break; // for other patterns return previous value
    }
    return value;
}

void Octoliner::pinModePort(uint16_t mask, uint8_t mode) {
    if (mode == INPUT) {
        writeCmd16BitData(PORT_MODE_INPUT, mask);
    } else if (mode == OUTPUT) {
        writeCmd16BitData(PORT_MODE_OUTPUT, mask);
    } else if (mode == INPUT_PULLUP) {
        writeCmd16BitData(PORT_MODE_PULLUP, mask);
    } else if (mode == INPUT_PULLDOWN) {
        writeCmd16BitData(PORT_MODE_PULLDOWN, mask);
    }
}

void Octoliner::pinMode(int pin, uint8_t mode) {
    uint16_t sendData = 1 << pin;
    pinModePort(sendData, mode);
}

void Octoliner::analogWrite(int pin, uint8_t pulseWidth) {
    uint16_t val = map(pulseWidth, 0, 255, 0, 65535);
    writeCmdPin16Val(ANALOG_WRITE, (uint8_t)pin, val, true);
}

int Octoliner::analogRead(int pin) {
    uint8_t mapper[8] = { 4, 5, 6, 8, 7, 3, 2, 1 };
    writeCmdPin(ANALOG_READ, (uint8_t)mapper[pin], true);
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
    Wire.write(temp); // Send MSB of 'uid' to the device

    temp = (uid >> 16) & 0xff;
    Wire.write(temp); // Send 2nd byte of 'uid' to the device

    temp = (uid >> 8) & 0xff;
    Wire.write(temp); // Send 3rd byte of 'uid' to the device

    temp = (uid)&0xff;
    Wire.write(temp); // Send LSB of 'uid' to the device
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
