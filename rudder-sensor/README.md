# Rudder Angle Sensor — ESP32 + AS5600 + SensESP

ESP32 firmware that reads rudder angle from an AS5600 magnetic sensor and
publishes it to a Signal K Server over WebSocket. Built with
[SensESP](https://github.com/SignalK/SensESP).

## Parts

- ESP32 dev board (WROOM-32 or similar)
- GY-AS5600 magnetic angle sensor breakout
- 6mm diametric magnet (attached to rudder post)

## Wiring

```
AS5600    ESP32
------    -----
VCC   →   3.3V
GND   →   GND
SDA   →   GPIO 21
SCL   →   GPIO 22
```

## Build & Flash

Requires [PlatformIO](https://platformio.org/).

```bash
cd rudder-sensor
pio run -t upload
pio device monitor        # watch serial output
```

## First Boot

1. The ESP32 tries to connect to WiFi SSID "bigair". If it can't connect (or
   no password is set), it creates an access point named **"rudder-angle"**
   (password: `thisisfine`).
2. Connect to the AP, open `http://192.168.4.1` in a browser.
3. Enter the WiFi SSID (`bigair`) and password, then save.
4. The device reboots, connects to the network, and discovers the Signal K
   Server via mDNS automatically.

## Signal K

- **Path:** `steering.rudderAngle`
- **Units:** radians (positive = starboard)
- **Update rate:** 10 Hz

The Signal K Server auto-discovers the ESP32 via mDNS — no manual server
configuration needed. Once connected, navigate to the Signal K data browser to
verify the `steering.rudderAngle` path is updating.

## Calibration

1. Center the rudder (tiller amidships).
2. Open the serial monitor (`pio device monitor`) and note the raw AS5600
   count being reported (0–4095).
3. Open the SensESP web UI at `http://rudder-angle.local` (or the device IP).
4. Go to **Rudder Zero Offset** and enter the raw count from step 2.
5. Save. The offset is persisted to flash and survives reboots.

Alternatively, if you have access to the Signal K data browser, watch
`steering.rudderAngle` — adjust the offset until centered rudder reads ~0.

## Troubleshooting

### Verify I2C connection

```bash
# On ESP32 serial monitor, check for "AS5600 found at 0x36" messages.
# Or use an I2C scanner sketch to confirm the sensor responds at 0x36.
```

If using a Raspberry Pi for testing I2C directly:

```bash
sudo apt install i2c-tools
i2cdetect -y 1
# Should show 36 in the grid
```

### Magnet orientation

The AS5600 requires a **diametrically magnetized** magnet centered 0.5–3mm
above the chip. If readings are erratic:

- Ensure the magnet is diametric (not axial)
- Check the air gap — too far away causes noisy readings
- Verify the magnet is centered over the IC

### No Signal K connection

- Confirm the Signal K Server is running: `http://<pi-ip>:3000`
- Check that the ESP32 and Pi are on the same "bigair" network
- Look at the ESP32 serial output for mDNS discovery messages
- Try setting the server address explicitly in the SensESP web UI

### Readings stuck at 0

- Check I2C wiring (SDA/SCL not swapped)
- Verify 3.3V power to the AS5600
- Ensure the magnet is present and close enough to the sensor
