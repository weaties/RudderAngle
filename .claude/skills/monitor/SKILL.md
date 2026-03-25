---
name: monitor
description: Open the ESP32 serial monitor to view logs. TRIGGER when the user says "monitor", "serial", "logs", "show output", or "what is the ESP32 doing".
---

# /monitor — ESP32 Serial Monitor

Connect to the ESP32 serial output to view runtime logs.

## Steps

1. Detect the serial port:
   ```bash
   ls /dev/cu.usbserial-* 2>/dev/null
   ```
   If no port found, tell the user the ESP32 isn't connected or the cable is
   charge-only.

2. Note: `pio device monitor` requires an interactive terminal and cannot be
   run from Claude Code's bash tool. Instead, tell the user to run it
   themselves:

   ```
   ! pio device monitor --port /dev/cu.usbserial-0001
   ```

   Or use the `!` prefix in the Claude Code prompt to run it in-session.

3. If the user pastes serial output, help interpret it:
   - **Garbage characters at boot** (`x�x`) — normal bootloader output at
     74880 baud; ignore these
   - **`NO_AP_FOUND`** — WiFi SSID not found; check spelling, case
     sensitivity, or 5 GHz vs 2.4 GHz
   - **`i2cWriteReadNonStop returned Error 263`** — I2C device not responding;
     check wiring and power
   - **`PENDING`** on Signal K auth — approve the device in Signal K admin UI
     at Security → Access Requests
   - **`Guru Meditation Error`** — crash; note the backtrace and check for
     memory issues or I2C bus contention
