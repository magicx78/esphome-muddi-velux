#pragma once

#ifdef USE_ARDUINO

#include "esphome/core/component.h"
#include "esphome/core/log.h"

#ifdef USE_SENSOR
#include "esphome/components/sensor/sensor.h"
#endif

#include "lib/WeatherSensor.h"

#include <map>
#include <vector>

namespace esphome {
namespace bresser_weather {

enum BresserField : uint8_t {
  FIELD_TEMPERATURE = 0,
  FIELD_HUMIDITY,
  FIELD_WIND_SPEED,
  FIELD_WIND_GUST,
  FIELD_WIND_DIRECTION,
  FIELD_RAIN,
  FIELD_UV,
  FIELD_LIGHT,
  FIELD_RSSI,
  FIELD_BATTERY_OK,
};

#ifdef USE_SENSOR
struct BresserSensorBinding {
  uint32_t sensor_id;
  BresserField field;
  sensor::Sensor *target;
};
#endif

class BresserWeatherHub : public Component {
 public:
  void set_sck_pin(int pin) { sck_pin_ = pin; }
  void set_miso_pin(int pin) { miso_pin_ = pin; }
  void set_mosi_pin(int pin) { mosi_pin_ = pin; }
  void set_cs_pin(int pin) { cs_pin_ = pin; }

#ifdef USE_SENSOR
  void register_sensor(uint32_t sensor_id, BresserField field, sensor::Sensor *target) {
    bindings_.push_back({sensor_id, field, target});
  }
#endif

  void setup() override;
  void loop() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::HARDWARE; }

 protected:
  void publish_sensor_(uint32_t sensor_id, BresserField field, float value);
  void process_decoded_();

  int sck_pin_{-1};
  int miso_pin_{-1};
  int mosi_pin_{-1};
  int cs_pin_{-1};

  WeatherSensor ws_;
  uint32_t last_packet_ms_{0};

#ifdef USE_SENSOR
  std::vector<BresserSensorBinding> bindings_;
#endif
};

}  // namespace bresser_weather
}  // namespace esphome

#endif  // USE_ARDUINO
