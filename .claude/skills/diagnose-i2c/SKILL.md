---
name: diagnose-i2c
description: Troubleshoot I2C bus issues with the AS5600 sensor and SSD1306 OLED. TRIGGER when the user reports I2C errors, display not working, sensor not reading, or garbled data.
---

# /diagnose-i2c — I2C Bus Troubleshooting

Systematic checklist for I2C problems on the ESP32 with AS5600 + SSD1306 OLED.

## Expected I2C devices

| Address | Device | Purpose |
|---------|--------|---------|
| 0x36 | AS5600 | Rudder angle sensor |
| 0x3C | SSD1306 | OLED display |

Both share GPIO 21 (SDA) and GPIO 22 (SCL). No external pull-ups needed —
both modules have built-in pull-ups.

## Diagnostic questions

Ask the user and work through these in order:

### 1. Power
- Is the ESP32 power LED on?
- Are both modules connected to 3.3V (NOT 5V)?
- Is the 3.3V rail actually powered? (A common breadboard issue — the power
  rails may not be connected end-to-end)

### 2. Wiring
- SDA on both modules goes to GPIO 21 (labeled D21 on most ESP32 boards)
- SCL on both modules goes to GPIO 22 (labeled D22)
- Check the pin labels on the OLED module — order varies between
  manufacturers. Common: GND, VCC, SCL, SDA (left to right)
- Ensure wires are firmly seated in the breadboard

### 3. Isolation test
- Disconnect the AS5600, leave only the OLED connected
- Power cycle the ESP32
- Does the OLED show "RudderAngle / Starting..."?
- If yes: reconnect AS5600 and test again
- If no: the OLED wiring is wrong

### 4. Serial log errors
- `i2cWriteReadNonStop returned Error 263` — I2C NACK, device not responding
  at that address. Check power and wiring.
- Errors in pairs every ~200ms — both sensor reads (10 Hz) and display
  updates (5 Hz) are failing; likely a bus-wide problem (power or SDA/SCL)
- Intermittent errors — possible bus contention or loose breadboard wire

### 5. If nothing works
- Try a different ESP32 board
- Try different jumper wires (breadboard wires go bad)
- Check solder joints on the module header pins
