# AGENTS.md

Guidance for AI agents working on this project.

## Context

This is an ESP32 firmware project for a marine rudder angle sensor. The code
runs on a microcontroller, not a server — there are no tests you can run
locally (it compiles to ARM via PlatformIO cross-compiler). Validation is done
by flashing to hardware and checking serial output.

## Before making changes

- Read the file you're modifying. The entire codebase is small (~300 lines of
  C++ across 4 files) so reading everything is fast and avoids mistakes.
- Understand the SensESP v3.x API before touching `main.cpp`. Key patterns:
  `SensESPAppBuilder`, `RepeatSensor<T>`, `LambdaTransform`, `connect_to()`
  chaining, `ConfigItem()` for web UI parameters.

## Building

```bash
cd rudder-sensor && pio run
```

This cross-compiles for ESP32. It does NOT produce a binary you can run on
your Mac. If the build succeeds, the firmware is at
`.pio/build/esp32dev/firmware.bin`.

To flash to a connected ESP32:
```bash
pio run -t upload --upload-port /dev/cu.usbserial-0001
```

The serial port name may vary. Check `ls /dev/cu.usb*` for available ports.

## Common pitfalls

- **WiFi credentials:** Never hardcode SSIDs or passwords in committed code.
  Always use `credentials.h` (gitignored). If you need to change WiFi config,
  edit `include/credentials.h` locally.
- **I2C bus sharing:** AS5600 (0x36) and OLED (0x3C) share the bus on GPIO
  21/22. Heavy I2C traffic from the display can cause sensor read errors.
  The display updates at 5 Hz (not 10 Hz) to reduce bus contention.
- **ESP32 WiFi is 2.4 GHz only.** If WiFi shows `NO_AP_FOUND`, the network
  may be 5 GHz.
- **SensESP requires C++17.** The `build_flags` include `-std=gnu++17` and
  `build_unflags` removes `-std=gnu++11`. Don't remove these.
- **PlatformIO mirror issues:** The Contabo CDN sometimes fails with SSL
  errors. See CLAUDE.md for the manual esptoolpy workaround.
- **`credentials.h` must exist to compile.** If the build fails with
  "credentials.h not found", copy `credentials.example.h` to `credentials.h`.

## What to test after changes

Since there are no automated tests, verify changes by:

1. `pio run` — confirm it compiles without errors
2. Flash to ESP32 and check serial output (`pio device monitor`)
3. Verify the OLED display shows correct data
4. Check that Signal K receives `steering.rudderAngle` deltas

## Scope

This repo is firmware only. The HelmLog side (ingesting rudder data from
Signal K, storing it, displaying on the instrument panel) lives in a separate
repo at https://github.com/weaties/helmlog.
