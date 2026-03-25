#ifndef DISPLAY_H
#define DISPLAY_H

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <Wire.h>
#include <cmath>

constexpr int SCREEN_WIDTH = 128;
constexpr int SCREEN_HEIGHT = 64;
constexpr uint8_t SSD1306_ADDR = 0x3C;

// Rudder gauge geometry
constexpr int GAUGE_CENTER_X = 64;
constexpr int GAUGE_CENTER_Y = 52;
constexpr int GAUGE_RADIUS = 30;
constexpr float MAX_DEFLECTION_RAD = 1.53588974f;  // 88°

static Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
static bool display_ok = false;

inline bool display_init() {
  display_ok = oled.begin(SSD1306_SWITCHCAPVCC, SSD1306_ADDR);
  if (display_ok) {
    oled.clearDisplay();
    oled.setTextColor(SSD1306_WHITE);
    oled.setTextSize(1);
    oled.setCursor(0, 0);
    oled.println("RudderAngle");
    oled.println("Starting...");
    oled.display();
  }
  return display_ok;
}

/**
 * Draw the status display:
 *   - Top bar: WiFi + SK connection status
 *   - Center: rudder angle in degrees (large text)
 *   - Bottom: rudder position gauge arc
 */
inline void display_update(float angle_rad, bool wifi_connected,
                           bool sk_connected) {
  if (!display_ok) return;

  float angle_deg = angle_rad * 57.2957795f;

  oled.clearDisplay();

  // -- Status bar (top) --
  oled.setTextSize(1);
  oled.setCursor(0, 0);
  oled.print("WiFi:");
  oled.print(wifi_connected ? "OK" : "--");
  oled.setCursor(72, 0);
  oled.print("SK:");
  oled.print(sk_connected ? "OK" : "--");

  // -- Angle readout (center, large) --
  oled.setTextSize(2);
  // Right-align the angle value
  char buf[8];
  snprintf(buf, sizeof(buf), "%+.1f", angle_deg);
  int16_t x1, y1;
  uint16_t w, h;
  oled.getTextBounds(buf, 0, 0, &x1, &y1, &w, &h);
  oled.setCursor((SCREEN_WIDTH - w) / 2, 14);
  oled.print(buf);

  // Degree symbol and label
  oled.setTextSize(1);
  oled.setCursor((SCREEN_WIDTH + w) / 2 + 2, 14);
  oled.print("o");

  // Port/Stbd labels
  oled.setCursor(0, 34);
  oled.print("P");
  oled.setCursor(SCREEN_WIDTH - 6, 34);
  oled.print("S");

  // -- Gauge arc (bottom) --
  // Draw arc from -88° to +88° (mapped to screen angles)
  for (int i = -88; i <= 88; i += 3) {
    float screen_angle = (270.0f - (float)i) * 0.0174533f;  // deg to rad
    int x = GAUGE_CENTER_X + (int)(GAUGE_RADIUS * cos(screen_angle));
    int y = GAUGE_CENTER_Y - (int)(GAUGE_RADIUS * sin(screen_angle));
    oled.drawPixel(x, y, SSD1306_WHITE);
  }

  // Tick marks at -45, 0, +45
  for (int tick : {-45, 0, 45}) {
    float screen_angle = (270.0f - (float)tick) * 0.0174533f;
    int x_inner = GAUGE_CENTER_X + (int)((GAUGE_RADIUS - 4) * cos(screen_angle));
    int y_inner = GAUGE_CENTER_Y - (int)((GAUGE_RADIUS - 4) * sin(screen_angle));
    int x_outer = GAUGE_CENTER_X + (int)((GAUGE_RADIUS + 2) * cos(screen_angle));
    int y_outer = GAUGE_CENTER_Y - (int)((GAUGE_RADIUS + 2) * sin(screen_angle));
    oled.drawLine(x_inner, y_inner, x_outer, y_outer, SSD1306_WHITE);
  }

  // Needle — line from center to arc edge at current angle
  float clamped = angle_deg;
  if (clamped > 88.0f) clamped = 88.0f;
  if (clamped < -88.0f) clamped = -88.0f;
  float needle_angle = (270.0f - clamped) * 0.0174533f;
  int nx = GAUGE_CENTER_X + (int)((GAUGE_RADIUS - 2) * cos(needle_angle));
  int ny = GAUGE_CENTER_Y - (int)((GAUGE_RADIUS - 2) * sin(needle_angle));
  oled.drawLine(GAUGE_CENTER_X, GAUGE_CENTER_Y, nx, ny, SSD1306_WHITE);

  oled.display();
}

#endif // DISPLAY_H
