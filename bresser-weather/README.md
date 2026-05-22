# esphome-bresser-weather

ESPHome external component that receives 868 MHz Bresser wireless weather
sensors (5-in-1 / 6-in-1 / 7-in-1) on an ESP32 board with an SX1276/RFM95W
radio. Wraps the excellent
[BresserWeatherSensorReceiver](https://github.com/matthias-bs/BresserWeatherSensorReceiver)
library by Matthias Prinke (vendored under `components/bresser_weather/lib/`,
pinned to **v0.41.0**).

Tested target hardware: **LilyGO TTGO LoRa32 T3 v1.6.1** (ESP32 + SX1276).

## Status

Early MVP. Decodes the 5/6/7-in-1 FSK weather telegrams, publishes
temperature, humidity, wind (avg / gust / direction), rain, UV, light and
RSSI as ESPHome sensors. Lightning, leakage, soil and air-quality decoders
are out of scope for the initial release.

## Quick start

1. Wire / use a LilyGO T3 v1.6.1 (pinout in `docs/pinout-t3-v1.6.1.md`).
2. Copy `example.yaml`, adjust WiFi secrets.
3. Leave the `sensor:` list empty on first flash. Watch the log for
   `RX id=0x...` and copy the IDs of sensors you care about into the YAML.
4. Re-flash with populated `sensor:` entries — the matching sensors appear
   in Home Assistant via the ESPHome API.

See `docs/finding-sensor-ids.md` for the discovery workflow.

## Configuration reference

```yaml
bresser_weather:
  id: bresser_hub
  sck_pin: 5
  miso_pin: 19
  mosi_pin: 27
  cs_pin: 18
  rst_pin: 23
  dio0_pin: 26    # SX1276 DIO0 -> RX-done IRQ
  dio1_pin: 33    # SX1276 DIO1 -> FIFO level (FSK)
  decoders: [5in1, 6in1, 7in1]   # default = all three
  max_sensors: 4                  # max simultaneously tracked sensors

sensor:
  - platform: bresser_weather
    sensor_id: 0x12345678         # hex ID from discovery log
    temperature: { name: "..." }
    humidity:    { name: "..." }
    wind_speed:  { name: "..." }
    wind_gust:   { name: "..." }
    wind_direction: { name: "..." }
    rain:        { name: "..." }
    uv:          { name: "..." }
    light:       { name: "..." }
    rssi:        { name: "..." }
    battery_ok:  { name: "..." }
```

All sub-keys under a `sensor_id` are optional — only declare what your
sensor actually transmits.

## Known limitations

- The vendored library still contains `while (true) delay(10);` paths
  inside `WeatherSensor::begin()` on radio init failure. If the radio
  hardware mis-initializes, the ESP32 will hang until the watchdog
  resets it instead of calling `mark_failed()`. Avoid wiring mistakes;
  patching this out is on the roadmap.
- The SPI bus is initialized via Arduino's global `SPI` object. Do not
  put other devices on the same bus from a separate `spi:` ESPHome
  component — they will fight over pin configuration.
- Only `SX1276` is wired up. CC1101 / SX1262 / LR1121 support is in the
  upstream library but currently not exposed in the YAML schema.
- Rain and lightning persistent counters (NVS Preferences in upstream)
  are disabled by default to avoid frequent NVS writes. Totals reset on
  reboot.

## Updating the vendored library

The library lives under `components/bresser_weather/lib/`. To bump:

```sh
git clone --depth 1 --branch vX.Y.Z https://github.com/matthias-bs/BresserWeatherSensorReceiver /tmp/upstream
cp /tmp/upstream/src/{WeatherSensor*.cpp,WeatherSensor.h,InitBoard.*,RainGauge.*,Lightning.*,RollingCounter.*,WeatherUtils.*} \
   components/bresser_weather/lib/
# Do NOT overwrite components/bresser_weather/lib/WeatherSensorCfg.h —
# that file is our ESPHome shim and must remain.
```

After bumping, update the version reference at the top of this README and
the `library.properties`-derived RadioLib pin in `__init__.py`.

## License

BSD-2-Clause, matching upstream. See `LICENSE`.
