/*
 * This example shows how to read analog values from Octoliner channels
 * and print them to Serial Monitor
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
  // Set sensitivity to 208 of 255
  octoliner.setSensitivity(208);
}

void loop() {
  // Loop through all eight channels
  for (uint8_t i = 0; i < 8; i++) {
    // For each channel output the value to Serial Monitor
    Serial.print(octoliner.analogRead(i));
    Serial.print("\t");
  }
  Serial.println();
  // Repeat in 0.2 seconds
  delay(200);
}
