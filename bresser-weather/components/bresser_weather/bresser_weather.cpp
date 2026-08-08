#ifdef USE_ARDUINO

#include "bresser_weather.h"

#include <SPI.h>

namespace esphome {
namespace bresser_weather {

static const char *const TAG = "bresser_weather";

void BresserWeatherHub::setup() {
  ESP_LOGCONFIG(TAG, "Setting up Bresser weather receiver…");

  if (sck_pin_ >= 0 && miso_pin_ >= 0 && mosi_pin_ >= 0 && cs_pin_ >= 0) {
    SPI.begin(sck_pin_, miso_pin_, mosi_pin_, cs_pin_);
  } else {
    ESP_LOGW(TAG, "SPI pins not fully configured, using Arduino defaults");
  }

  int16_t state = ws_.begin();
  if (state != 0) {
    ESP_LOGE(TAG, "Radio init failed, code=%d — marking component as failed", state);
    this->mark_failed();
    return;
  }
  ESP_LOGI(TAG, "Radio init OK on %s", RECEIVER_CHIP);
}

void BresserWeatherHub::loop() {
  int status = ws_.getMessage();
  if (status != DECODE_OK) {
    return;
  }
  last_packet_ms_ = millis();
  this->process_decoded_();
}

void BresserWeatherHub::process_decoded_() {
  for (size_t i = 0; i < ws_.sensor.size(); i++) {
    auto &s = ws_.sensor[i];
    if (!s.valid) {
      continue;
    }

    ESP_LOGD(TAG, "RX id=0x%08X type=%u chan=%u dec=%u rssi=%.1f batt_ok=%d",
             s.sensor_id, s.s_type, s.chan, s.decoder, s.rssi, s.battery_ok);

    if (s.s_type == SENSOR_TYPE_WEATHER0 || s.s_type == SENSOR_TYPE_WEATHER1) {
      if (s.w.temp_ok) {
        ESP_LOGD(TAG, "  temp=%.1f°C", s.w.temp_c);
        this->publish_sensor_(s.sensor_id, FIELD_TEMPERATURE, s.w.temp_c);
      }
      if (s.w.humidity_ok) {
        ESP_LOGD(TAG, "  humidity=%u%%", s.w.humidity);
        this->publish_sensor_(s.sensor_id, FIELD_HUMIDITY, (float) s.w.humidity);
      }
      if (s.w.wind_ok) {
        ESP_LOGD(TAG, "  wind avg=%.1f gust=%.1f dir=%.0f",
                 s.w.wind_avg_meter_sec, s.w.wind_gust_meter_sec, s.w.wind_direction_deg);
        this->publish_sensor_(s.sensor_id, FIELD_WIND_SPEED, s.w.wind_avg_meter_sec);
        this->publish_sensor_(s.sensor_id, FIELD_WIND_GUST, s.w.wind_gust_meter_sec);
        this->publish_sensor_(s.sensor_id, FIELD_WIND_DIRECTION, s.w.wind_direction_deg);
      }
      if (s.w.rain_ok) {
        ESP_LOGD(TAG, "  rain=%.1fmm", s.w.rain_mm);
        this->publish_sensor_(s.sensor_id, FIELD_RAIN, s.w.rain_mm);
      }
      if (s.w.uv_ok) {
        this->publish_sensor_(s.sensor_id, FIELD_UV, s.w.uv);
      }
      if (s.w.light_ok) {
        this->publish_sensor_(s.sensor_id, FIELD_LIGHT, s.w.light_lux);
      }
    }

    this->publish_sensor_(s.sensor_id, FIELD_RSSI, s.rssi);
    this->publish_sensor_(s.sensor_id, FIELD_BATTERY_OK, s.battery_ok ? 1.0f : 0.0f);

    s.valid = false;
  }
}

void BresserWeatherHub::publish_sensor_(uint32_t sensor_id, BresserField field, float value) {
#ifdef USE_SENSOR
  for (auto &b : bindings_) {
    if (b.sensor_id == sensor_id && b.field == field && b.target != nullptr) {
      b.target->publish_state(value);
    }
  }
#endif
}

void BresserWeatherHub::dump_config() {
  ESP_LOGCONFIG(TAG, "Bresser Weather Hub:");
  ESP_LOGCONFIG(TAG, "  Radio chip: %s", RECEIVER_CHIP);
  ESP_LOGCONFIG(TAG, "  SPI pins: SCK=%d MISO=%d MOSI=%d CS=%d", sck_pin_, miso_pin_, mosi_pin_, cs_pin_);
  ESP_LOGCONFIG(TAG, "  Radio pins: IRQ=%d RST=%d GPIO/DIO1=%d", PIN_RECEIVER_IRQ, PIN_RECEIVER_RST, PIN_RECEIVER_GPIO);
#ifdef USE_SENSOR
  ESP_LOGCONFIG(TAG, "  Registered sensor bindings: %u", (unsigned) bindings_.size());
#endif
}

}  // namespace bresser_weather
}  // namespace esphome

#endif  // USE_ARDUINO
