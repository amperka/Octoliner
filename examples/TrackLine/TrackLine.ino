/*
 * This example shows how to track a line under the sensor by getting values
 * in the [-1.0; +1.0] range and print them to Serial Monitor.
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
  // Find the line and log its position to Serial
  Serial.println(octoliner.trackLine());
  // Repeat in 0.2 seconds
  delay(200);
}
