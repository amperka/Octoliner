# Octoliner

Arduino library to interface with the Amperka [Octoliner](https://my.amperka.com/modules/octoliner) 8-channel line sensor.

## Installation

- Download the library “Source code (zip)” from the [Releases](https://github.com/amperka/Octoliner/releases) page on GitHub.
- In Arduino IDE hit “Sketch → Include Library → Add .ZIP Library...” and select the file downloaded.

## API

Quickstart example:

```cpp
// Include two libraries required to work with the module
#include <Wire.h>
#include <Octoliner.h>

// Sensor on the standard bus and address
Octoliner octoliner;

void setup() {
  Serial.begin(115200);
  octoliner.begin();

  // Set sensitivity to 200 of 255
  octoliner.setSensitivity(200);
}

void loop() {
  // Every 0.5 seconds...
  delay(500);

  // ...scan all 8 channels and output
  // the values to Serial Monitor
  for (int i = 0; i < 8; i++) {
    Serial.print(octoliner.analogRead(i));
    Serial.print("\t"); // separate with TABs
  }

  Serial.println(); // line feed at the end
}
```

See full [API reference in API.md](./API.md).
