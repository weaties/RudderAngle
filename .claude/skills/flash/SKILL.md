---
name: flash
description: Build and flash the ESP32 firmware via PlatformIO. TRIGGER when the user says "flash", "upload", "build and upload", or "deploy to ESP32". Detects the serial port automatically.
---

# /flash — Build & Flash ESP32

Build the firmware and upload it to the connected ESP32.

## Steps

1. Verify `credentials.h` exists:
   ```bash
   test -f rudder-sensor/include/credentials.h || { echo "ERROR: credentials.h missing — copy credentials.example.h"; exit 1; }
   ```

2. Detect the serial port:
   ```bash
   ls /dev/cu.usbserial-* 2>/dev/null
   ```
   If no port found, tell the user to check their USB cable (charge-only cables
   won't work) and that the ESP32 must be plugged in via USB.

3. Build and upload:
   ```bash
   cd rudder-sensor && pio run -t upload --upload-port <detected-port>
   ```

4. If the build fails:
   - Missing `credentials.h` → tell user to copy from example
   - PlatformIO SSL/mirror errors → see CLAUDE.md for esptoolpy workaround
   - C++ errors → read the failing file and fix

5. After successful flash, offer to open the serial monitor:
   ```bash
   pio device monitor --port <detected-port>
   ```

## Arguments

- `/flash` — auto-detect port, build and upload
- `/flash monitor` — flash then open serial monitor
