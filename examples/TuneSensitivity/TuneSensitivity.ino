/*
 * This example shows how to adjust Octoliner sensitivity with a potentiometer.
 * The values coming from Octoliner at the given sensitivity are output
 * to Serial Monitor as bars made with #’s
 *
 * #
 * #
 * #
 * ##########
 * ##########
 * ######
 * #
 * #
 *
 */

// Include the interfacing library
#include <Octoliner.h>

// Define the pin with the tuning potentiometer
#define PIN_SENSITIVITY   A0

// Create an object to work with Octoliner with
// the address 42 (decimal, default)
Octoliner octoliner(42);

void setup() {
  // Open Serial-port with 115200 baud rate. Do not forget to set the
  // same rate in Serial Monitor
  Serial.begin(115200);
  // Configure hardware and set up the sensor
  octoliner.begin();
}

void loop() {
  // Get the value from potentiometer in [0; 255] range
  int16_t sensitivity = map(analogRead(PIN_SENSITIVITY), 0, 1024, 0, 255);

  // Set it to Octoliner
  octoliner.setSensitivity(sensitivity);

  // Scan through all eight channels
  for (uint8_t i = 0; i < 8; i++) {
    // Map the range 0…4096 to 0…32
    int16_t sharpCount = octoliner.analogRead(i) >> 7;
    // Output the bars with # symbols
    for (int16_t y = 0; y <= sharpCount; y++) {
      Serial.print('#');
    }
    Serial.println();
  }
  // Add a few new lines to delimit this pass output
  Serial.println("\r\n\r\n");
  // Repeat after 0.1 second
  delay(100);
}
