# Waveshare 4.26" E-Paper Display Test for ESP32-S3

This Arduino sketch tests a Waveshare 4.26-inch E-Paper Display HAT (800x480, Black/White, SPI Interface) connected to an ESP32-S3-N16R8 development board.

## Hardware Requirements

- ESP32-S3-DevKitC-1 (or compatible ESP32-S3 board with 16MB Flash, 8MB PSRAM)
- Waveshare 4.26-inch E-Paper Display HAT (800x480, Black/White)
- 9-pin connection cable
- Stable 3.3V power supply (E-Paper displays can draw significant current during refresh)

## Pin Connections

| E-Paper Pin | ESP32-S3 GPIO | Function |
|-------------|---------------|----------|
| VCC         | 3.3V          | Power supply |
| GND         | GND           | Ground |
| DIN         | GPIO 11       | SPI MOSI (Data In) |
| CLK         | GPIO 12       | SPI Clock |
| CS          | GPIO 10       | SPI Chip Select |
| DC          | GPIO 8        | Data/Command control |
| RST         | GPIO 9        | Reset |
| BUSY        | GPIO 7        | Busy signal |
| PWR         | GPIO 6        | Display power control (Active HIGH) |

## Software Setup

### 1. Install Arduino IDE
- Download and install the latest Arduino IDE from [arduino.cc](https://www.arduino.cc/en/software)

### 2. Add ESP32-S3 Board Support
1. Open Arduino IDE
2. Go to **File → Preferences**
3. Add this URL to "Additional Board Manager URLs":
   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
4. Go to **Tools → Board → Board Manager**
5. Search for "esp32" and install "esp32 by Espressif Systems"

### 3. Install Required Library
1. Go to **Tools → Manage Libraries**
2. Search for "GxEPD2" by Jean-Marc Zingg
3. Install the latest version

### 4. Configure Board Settings
1. Select **Tools → Board → ESP32 Arduino → ESP32S3 Dev Module**
2. Configure the following settings:
   - **Flash Size:** 16MB (128Mb)
   - **Partition Scheme:** Default 4MB with spiffs
   - **PSRAM:** OPI PSRAM
   - **Upload Speed:** 921600
   - **USB Mode:** Hardware CDC and JTAG

## Usage

1. Connect the E-Paper display to your ESP32-S3 using the pin connections above
2. Open `Waveshare_426_EPaper_Test.ino` in Arduino IDE
3. Select the correct COM port under **Tools → Port**
4. Click Upload
5. Open Serial Monitor (115200 baud) to see debug output

## Test Sequence

The sketch performs the following tests:

1. **Display Initialization** - Powers on and initializes the display
2. **Clear to White** - Prevents ghosting by clearing the display
3. **Text Display (0°)** - Shows text in various font sizes
4. **Text Display (90°)** - Tests rotation functionality
5. **Geometric Shapes** - Draws rectangles, circles, lines, and triangles
6. **Partial Refresh** - Updates a small area with a counter
7. **Final Clear** - Clears display and powers off

## Troubleshooting

### Display Not Responding
- Verify all connections, especially power and ground
- Check PWR pin is HIGH (measure with multimeter)
- Ensure stable 3.3V power supply (use external supply if needed)
- Try adding 10kΩ pull-up resistors on CS, DC, and RST lines

### Corrupted Display Output
- Check power supply stability (E-Paper needs high current during refresh)
- Verify SPI connections (especially CLK and MOSI)
- Try reducing SPI speed in the code
- Ensure proper grounding between ESP32 and display

### Busy Timeout Errors
- Check BUSY pin connection
- Some displays use inverted BUSY logic (LOW when busy)
- Modify `waitForNotBusy()` function if needed

### Wrong Display Driver
If the display doesn't work with `GxEPD2_420`, try these alternatives:
```cpp
// Option 1: Specific 4.26" driver (if available in your GxEPD2 version)
GxEPD2_BW<GxEPD2_426_GDEQ0426T82, GxEPD2_426_GDEQ0426T82::HEIGHT> display(...);

// Option 2: Generic 4.2" driver
GxEPD2_BW<GxEPD2_420_GDEW042T2, GxEPD2_420_GDEW042T2::HEIGHT> display(...);

// Option 3: Check GxEPD2 examples folder for your specific model
```

### PWR Pin Behavior
- Most Waveshare displays: PWR HIGH = Display ON
- Some models may use inverted logic: PWR LOW = Display ON
- If display doesn't respond, try inverting PWR logic in the code
- Consult your specific display model's datasheet

## Power Consumption Notes

- E-Paper displays consume power only during refresh
- After refresh, the image persists without power
- The PWR pin can be used to completely power down the display
- Typical refresh current: 20-50mA
- Standby current: <0.1mA

## Additional Resources

- [GxEPD2 Library Documentation](https://github.com/ZinggJM/GxEPD2)
- [Waveshare E-Paper Wiki](https://www.waveshare.com/wiki/Main_Page#E-Paper)
- [ESP32-S3 Technical Reference](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/)

## License

This test sketch is provided as-is for educational and testing purposes. 