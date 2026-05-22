"""Child-sensor platform for bresser_weather.

Binds individual Bresser sensor IDs (transmitter hardware IDs) to ESPHome
Sensor entities. Add one entry per physical sensor in range; unknown IDs are
logged at DEBUG so you can discover them on first flash.
"""

import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import (
    DEVICE_CLASS_TEMPERATURE,
    DEVICE_CLASS_HUMIDITY,
    DEVICE_CLASS_WIND_SPEED,
    DEVICE_CLASS_SIGNAL_STRENGTH,
    DEVICE_CLASS_ILLUMINANCE,
    STATE_CLASS_MEASUREMENT,
    STATE_CLASS_TOTAL_INCREASING,
    UNIT_CELSIUS,
    UNIT_PERCENT,
    UNIT_METER_PER_SECOND,
    UNIT_DEGREES,
    UNIT_MILLIMETER,
    UNIT_LUX,
    UNIT_DECIBEL_MILLIWATT,
    UNIT_EMPTY,
)

from . import BresserWeatherHub, BresserField

CONF_BRESSER_WEATHER_ID = "bresser_weather_id"
CONF_SENSOR_ID = "sensor_id"
CONF_TEMPERATURE = "temperature"
CONF_HUMIDITY = "humidity"
CONF_WIND_SPEED = "wind_speed"
CONF_WIND_GUST = "wind_gust"
CONF_WIND_DIRECTION = "wind_direction"
CONF_RAIN = "rain"
CONF_UV = "uv"
CONF_LIGHT = "light"
CONF_RSSI = "rssi"
CONF_BATTERY_OK = "battery_ok"

FIELDS = {
    CONF_TEMPERATURE: (
        "FIELD_TEMPERATURE",
        sensor.sensor_schema(
            unit_of_measurement=UNIT_CELSIUS,
            accuracy_decimals=1,
            device_class=DEVICE_CLASS_TEMPERATURE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
    ),
    CONF_HUMIDITY: (
        "FIELD_HUMIDITY",
        sensor.sensor_schema(
            unit_of_measurement=UNIT_PERCENT,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_HUMIDITY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
    ),
    CONF_WIND_SPEED: (
        "FIELD_WIND_SPEED",
        sensor.sensor_schema(
            unit_of_measurement=UNIT_METER_PER_SECOND,
            accuracy_decimals=1,
            device_class=DEVICE_CLASS_WIND_SPEED,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
    ),
    CONF_WIND_GUST: (
        "FIELD_WIND_GUST",
        sensor.sensor_schema(
            unit_of_measurement=UNIT_METER_PER_SECOND,
            accuracy_decimals=1,
            device_class=DEVICE_CLASS_WIND_SPEED,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
    ),
    CONF_WIND_DIRECTION: (
        "FIELD_WIND_DIRECTION",
        sensor.sensor_schema(
            unit_of_measurement=UNIT_DEGREES,
            accuracy_decimals=0,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
    ),
    CONF_RAIN: (
        "FIELD_RAIN",
        sensor.sensor_schema(
            unit_of_measurement=UNIT_MILLIMETER,
            accuracy_decimals=1,
            state_class=STATE_CLASS_TOTAL_INCREASING,
        ),
    ),
    CONF_UV: (
        "FIELD_UV",
        sensor.sensor_schema(
            unit_of_measurement=UNIT_EMPTY,
            accuracy_decimals=1,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
    ),
    CONF_LIGHT: (
        "FIELD_LIGHT",
        sensor.sensor_schema(
            unit_of_measurement=UNIT_LUX,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_ILLUMINANCE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
    ),
    CONF_RSSI: (
        "FIELD_RSSI",
        sensor.sensor_schema(
            unit_of_measurement=UNIT_DECIBEL_MILLIWATT,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_SIGNAL_STRENGTH,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
    ),
    CONF_BATTERY_OK: (
        "FIELD_BATTERY_OK",
        sensor.sensor_schema(
            unit_of_measurement=UNIT_EMPTY,
            accuracy_decimals=0,
        ),
    ),
}


def _sensor_id_validator(value):
    if isinstance(value, int):
        if value < 0 or value > 0xFFFFFFFF:
            raise cv.Invalid("sensor_id must fit in 32 bits")
        return value
    return cv.hex_int(value)


CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_BRESSER_WEATHER_ID): cv.use_id(BresserWeatherHub),
        cv.Required(CONF_SENSOR_ID): _sensor_id_validator,
        **{cv.Optional(key): schema for key, (_, schema) in FIELDS.items()},
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_BRESSER_WEATHER_ID])
    sensor_id = config[CONF_SENSOR_ID]
    for key, (field_enum, _) in FIELDS.items():
        if key not in config:
            continue
        sens = await sensor.new_sensor(config[key])
        cg.add(hub.register_sensor(sensor_id, getattr(BresserField, field_enum), sens))
