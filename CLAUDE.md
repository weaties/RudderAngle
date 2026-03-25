# CLAUDE.md

## Project overview

ESP32 firmware (C++/Arduino) that reads rudder angle from an AS5600 magnetic
sensor and publishes it to a Signal K Server. Built with PlatformIO and the
SensESP framework.

## Build & flash

```bash
cd rudder-sensor
pio run -t upload --upload-port /dev/cu.usbserial-0001
pio device monitor --port /dev/cu.usbserial-0001
```

If `pio` is not installed: `uv tool install platformio`

PlatformIO's Contabo CDN mirrors intermittently fail with SSL errors. If
`tool-esptoolpy` won't download, manually install it from the GitHub release:
```bash
curl -L -o /tmp/esptoolpy.tar.gz \
  "https://github.com/espressif/esptool/releases/download/v4.11.0/esptool-v4.11.0-macos-arm64.tar.gz"
mkdir -p ~/.platformio/packages/tool-esptoolpy
tar xzf /tmp/esptoolpy.tar.gz -C ~/.platformio/packages/tool-esptoolpy --strip-components=1
```
Then create `~/.platformio/packages/tool-esptoolpy/esptool.py` as a Python
wrapper that calls the standalone binary.

## Project structure

```
rudder-sensor/
├── platformio.ini                 # Build config, library deps
├── src/main.cpp                   # SensESP app wiring
├── include/
│   ├── as5600_sensor.h            # I2C sensor read + radians conversion
│   ├── display.h                  # SSD1306 OLED rendering
│   ├── credentials.h              # WiFi creds (gitignored)
│   └── credentials.example.h      # Template for credentials.h
└── README.md                      # Wiring, setup, calibration
```

## Key details

- **Sensor:** AS5600 at I2C `0x36`, raw 12-bit angle (0–4095)
- **Display:** SSD1306 at I2C `0x3C`, 128x64, GME12864-11 module
- **I2C pins:** SDA = GPIO 21, SCL = GPIO 22
- **Signal K path:** `steering.rudderAngle` (radians, + = starboard)
- **Clamp:** ±88 degrees — anything beyond is noise or error
- **Sample rate:** sensor reads at 10 Hz, display updates at 5 Hz
- **Smoothing:** 5-sample moving average (configurable via SensESP web UI)
- **Calibration:** zero offset stored in flash, editable at `http://rudder-angle.local`
- **WiFi:** SSID and password come from `include/credentials.h`
- **ESP32 only supports 2.4 GHz WiFi** — will not see 5 GHz networks

## Conventions

- PlatformIO project, not Arduino IDE — no `.ino` files
- C++17 (`-std=gnu++17` in build flags)
- SensESP v3.x API: `SensESPAppBuilder`, `RepeatSensor`, `LambdaTransform`,
  `connect_to()` chaining
- WiFi credentials must never be committed — `credentials.h` is gitignored
- Header-only sensor and display code in `include/` (inline functions)

## Related

- **HelmLog** (`/Users/dweatbrook/src/helmlog/`): Sailing data platform that
  consumes this sensor's data via Signal K. PR #420 adds rudder angle support.
- **Signal K Server** runs on a Raspberry Pi on the boat's "BigAir2.4" network.
