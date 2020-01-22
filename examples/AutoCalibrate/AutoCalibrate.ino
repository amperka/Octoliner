/*
 * This example shows how to auto-adjust Octoliner sensitivity.
 */

// Include the interfacing library
#include <Octoliner.h>

// Create an object to work with Octoliner with
// the address 42 (decimal, default)
Octoliner octoliner(42);

void setup() {
    // Open Serial-port with 115200 baud rate. Do not forget to set the
    // same rate in Serial Monitor
    Serial.begin(115200);
    // Configure hardware and set up the sensor
    octoliner.begin();
    delay(1000);

    Serial.print("Optimal sensitivity: ");
    octoliner.optimizeSensitivityOnBlack();
    Serial.print(octoliner.getSensitivity());
    Serial.println("\r\nPress any character button, and press Enter for calculate...\r\n");
}

void loop() {
    if (Serial.available() > 0) {
        int incomingByte = Serial.read();
        Serial.print("Optimal sensitivity: ");
        octoliner.optimizeSensitivityOnBlack();
        Serial.print(octoliner.getSensitivity());
        Serial.println("\r\n\r\n");
    }
}
