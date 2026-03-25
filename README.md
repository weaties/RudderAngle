# RudderAngle

DIY rudder angle sensor for sailboats using an ESP32, AS5600 magnetic encoder,
and SSD1306 OLED display. Publishes to [Signal K](https://signalk.org/) over
WebSocket so any Signal K consumer (chart plotters, dashboards,
[HelmLog](https://github.com/weaties/helmlog)) can use the data.

## How it works

A diametric magnet on the rudder post rotates over an AS5600 Hall-effect
sensor. The ESP32 reads the 12-bit angle at 10 Hz, converts to radians, and
publishes `steering.rudderAngle` to Signal K Server via the
[SensESP](https://github.com/SignalK/SensESP) framework. A small OLED shows
the angle, a gauge needle, and WiFi/SK connection status.

```
Magnet on rudder post
        │
    AS5600 (I2C 0x36) ──┐
                         ├── ESP32 ──WiFi──▶ Signal K Server ──▶ HelmLog / Grafana / etc.
    SSD1306 OLED (0x3C) ─┘
```

## Quick start

```bash
cd rudder-sensor
cp include/credentials.example.h include/credentials.h
# Edit credentials.h with your WiFi SSID and password
pio run -t upload
```

See [rudder-sensor/README.md](rudder-sensor/README.md) for full wiring,
calibration, and troubleshooting instructions.

## Hardware

| Part | Purpose |
|------|---------|
| ESP32 dev board (WROOM-32) | Microcontroller + WiFi |
| GY-AS5600 breakout | 12-bit magnetic angle sensor |
| 6 mm diametric magnet | Attaches to rudder post |
| GME12864-11 OLED (128x64) | Status display |

All components share a single I2C bus on GPIO 21 (SDA) / GPIO 22 (SCL).

## Signal K

- **Path:** `steering.rudderAngle`
- **Units:** radians (positive = starboard)
- **Range:** ±88 degrees (±1.536 rad)
- **Rate:** 10 Hz

The ESP32 auto-registers with Signal K Server via mDNS. No server-side
configuration needed.

## HelmLog integration

[HelmLog PR #420](https://github.com/weaties/helmlog/pull/420) adds rudder
angle support — subscribes to `steering.rudderAngle`, stores readings in
SQLite, and displays **RDR** on the instrument panel.

## License

MIT
