#ifndef AS5600_SENSOR_H
#define AS5600_SENSOR_H

#include <Wire.h>

// AS5600 I2C address and raw angle register
constexpr uint8_t AS5600_ADDR = 0x36;
constexpr uint8_t AS5600_RAW_ANGLE_REG = 0x0C;

// 12-bit range to radians: 2*PI / 4096
constexpr float AS5600_COUNTS_TO_RAD = 6.283185307f / 4096.0f;

// Max rudder angle clamp: ±45° in radians
constexpr float RUDDER_ANGLE_MAX_RAD = 0.785398163f;

/**
 * Read the raw 12-bit angle from the AS5600 over I2C.
 * Returns -1 on communication failure.
 */
inline int16_t as5600_read_raw() {
  Wire.beginTransmission(AS5600_ADDR);
  Wire.write(AS5600_RAW_ANGLE_REG);
  if (Wire.endTransmission(false) != 0) {
    return -1;
  }
  if (Wire.requestFrom(AS5600_ADDR, (uint8_t)2) != 2) {
    return -1;
  }
  uint16_t high = Wire.read();
  uint16_t low = Wire.read();
  return (int16_t)((high << 8 | low) & 0x0FFF);
}

/**
 * Convert a raw 12-bit AS5600 reading to a centered, clamped rudder angle
 * in radians. Applies the zero offset, wraps to [-PI, PI], and clamps to
 * ±RUDDER_ANGLE_MAX_RAD.
 *
 * @param raw         Raw 12-bit value (0-4095)
 * @param zero_offset Raw count at center position (set during calibration)
 * @return Rudder angle in radians (+ = starboard)
 */
inline float as5600_to_rudder_angle(int16_t raw, float zero_offset) {
  float angle_rad = ((float)raw - zero_offset) * AS5600_COUNTS_TO_RAD;

  // Wrap to [-PI, PI]
  if (angle_rad > 3.14159265f) {
    angle_rad -= 6.28318530f;
  } else if (angle_rad < -3.14159265f) {
    angle_rad += 6.28318530f;
  }

  // Clamp to ±45°
  if (angle_rad > RUDDER_ANGLE_MAX_RAD) {
    angle_rad = RUDDER_ANGLE_MAX_RAD;
  } else if (angle_rad < -RUDDER_ANGLE_MAX_RAD) {
    angle_rad = -RUDDER_ANGLE_MAX_RAD;
  }

  return angle_rad;
}

#endif // AS5600_SENSOR_H
