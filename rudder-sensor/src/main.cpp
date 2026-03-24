#include <Wire.h>

#include "as5600_sensor.h"
#include "display.h"
#include "sensesp/signalk/signalk_output.h"
#include "sensesp/transforms/lambda_transform.h"
#include "sensesp/transforms/moving_average.h"
#include "sensesp/ui/config_item.h"
#include "sensesp_app_builder.h"

using namespace sensesp;

// Latest smoothed rudder angle for display
static float current_angle_rad = 0.0f;

void setup() {
  SetupLogging();

  SensESPAppBuilder builder;
  sensesp_app = builder.set_hostname("rudder-angle")
                    ->set_wifi_client("bigair", "")
                    ->get_app();

  // Initialize I2C: SDA=21, SCL=22 (shared by AS5600 and OLED)
  Wire.begin(21, 22);

  // Initialize the OLED display
  display_init();

  // Read AS5600 at 10 Hz (100 ms interval)
  auto* sensor = new RepeatSensor<float>(100, []() -> float {
    int16_t raw = as5600_read_raw();
    if (raw < 0) {
      return 0.0f;  // I2C error — report center
    }
    return (float)raw;
  });

  // Apply zero-offset calibration (configurable via web UI)
  const ParamInfo* offset_info = new ParamInfo[1]{
      {"zero_offset", "Raw count at center position (0-4095)"}};

  auto* calibration = new LambdaTransform<float, float, float>(
      [](float raw, float offset) -> float {
        return as5600_to_rudder_angle((int16_t)raw, offset);
      },
      0.0f,                        // default zero offset
      offset_info,
      "/rudder/zero_offset"        // config path — enables persistence
  );

  ConfigItem(calibration)
      ->set_title("Rudder Zero Offset")
      ->set_description(
          "Set to the raw AS5600 count when the rudder is centered. "
          "Center the rudder, note the raw reading from the serial log, "
          "and enter it here.")
      ->set_sort_order(1000);

  // Smooth jitter with a 5-sample moving average
  auto* smooth = new MovingAverage(5, 1.0, "/rudder/smoothing");
  ConfigItem(smooth)
      ->set_title("Rudder Smoothing")
      ->set_description("Moving average window size for jitter reduction.")
      ->set_sort_order(1100);

  // Capture the smoothed angle for the display
  auto* display_tap = new LambdaTransform<float, float>(
      [](float angle) -> float {
        current_angle_rad = angle;
        return angle;
      });

  // Wire the pipeline: sensor → calibrate → smooth → display tap → Signal K
  sensor->connect_to(calibration)
      ->connect_to(smooth)
      ->connect_to(display_tap)
      ->connect_to(new SKOutputFloat(
          "steering.rudderAngle", "/rudder/sk",
          new SKMetadata("rad", "Rudder Angle")));

  // Update the OLED display at 5 Hz (every 200ms)
  event_loop()->onRepeat(200, []() {
    bool wifi_ok = WiFi.status() == WL_CONNECTED;
    bool sk_ok = sensesp_app->get_ws_client() &&
                 sensesp_app->get_ws_client()->is_connected();
    display_update(current_angle_rad, wifi_ok, sk_ok);
  });
}

void loop() { event_loop()->tick(); }
