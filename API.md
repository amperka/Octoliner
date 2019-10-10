# Octoliner API

## `class Octoliner`

Create an object of type `Octoliner` to communicate with a particular Octoliner board.

### `Octoliner(uint8_t i2cAddress = 42)`

Constructs a new object. The `i2caddress` specifies the board I²C address which is 42 factory-default but can be changed programmatically.

### `void begin()`
### `void begin(TwoWire* wire)`

Initializes the given interface, prepares the board for communication. If the `wire` argument is omitted, the connection is started on hardware I2C#0. The method initializes the specified or default TwoWire interface. If it was initialized before this method reinitializes it.

Call this method once, in your `setup()`.

### `void setSensitivity(uint8_t sense)`

Change load resistance value for IR sensors. An increase in load resistance leads to decrease IR sensor sensitivity. 
`0` corresponds to the minimal sensitivity, and `255` corresponds to the maximal sensitivity.

For Amperka Dragster (6mm Octoliner to track distance) you can use values [230; 250]. If Octoliner closer to track you will be forced reduce value.

### `uint16_t analogRead(uint8_t channel)`

Reads the lightness signal from the given `channel`. Returns an integer in the range [0; 4096] as a result. The `channel` value is expected to be from `0` to `7`. The channel numbers are printed on the board.

- The more light reflected (more like white), the closer the value to `0`.
- The less light reflected (more like black), the closer the value to `4096`.

### `void analogReadAll(uint16_t* analogValues)`

Reads all 8 channels to user array rawData.

### `virtual uint8_t patternMaker(int* rawData)`

Make 8-bit pattern from rawData array - one bit for one channel. "1" is dark and "0" is light. User can replace this method to own algorythm. For example, for using other trashold level.

### `virtual float patternDecoder(uint8_t pattern)`

Interpret channels pattern as line position in range from `-1.0` (on the left extreme) to `+1.0` (on the right extreme).
When the line is under the sensor center, the return value is `0.0`.

The method is implemented in a way which makes it suitable to track a line of ~17 to 25 mm in thickness. The result is not fairly analog but comes at increments of ⅛. Nevertheless the return value is a good input for a PID-controller.

If the current sensor reading does not allow understanding of the line position (some trash under the sensor or gone out of track), the previous value is returned. The starting value is `0.0`.

User can replace this method to own, for other line thickness or other mind.

### `uint8_t digitalReadAll()`

Reads all 8 channels raw data and interpret it to line position pattern.


### `float trackLine()`
### `float trackLine(uint8_t pattern)`
### `float trackLine(uint16_t* analogValues)`

Estimates line position under the sensor and returns the value in the range from `-1.0` (on the left extreme) to `+1.0` (on the right extreme). When the line is under the sensor center, the return value is `0.0`.

If no argument is provided, the method reads all channels. Otherwise, the data provided is used as input data. The latter is useful when you use the data for other purposes as well (e.g., to visualize it on a display) because it avoids re-reading.

If the current sensor reading does not allow understanding of the line position (some trash under the sensor or gone out of track), the previous successful estimation (which is cached) is returned. The starting value is `0.0`.

### `float trackLine(int* rawData)`

Estimates line position under the sensor and returns the value in the range from `-1.0` (on the left extreme) to `+1.0` (on the right extreme). When the line is under the sensor center, the return value is `0.0`.

Use `pattermMaker` for convert `rawData` to pattern and `patternDecoder` to interpret this pattern.

### `float trackLine(uint8_t pattern)`

Estimates line position under the sensor and returns the value in the range from `-1.0` (on the left extreme) to `+1.0` (on the right extreme). When the line is under the sensor center, the return value is `0.0`.

Use `patternDecoder` for interpret given pattern.

### `void changeAddress(uint8_t newAddress)`

Changes the I²C address of the module. The change is in effect only while the board is powered on. If you want to save it permanently call the `saveAddress` method.

### `void saveAddress()`

Permanently saves the current board I²C address.
