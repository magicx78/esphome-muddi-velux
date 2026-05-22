"""ESPHome external component for Bresser 868 MHz weather sensors.

Wraps the BresserWeatherSensorReceiver library (vendored under lib/),
configured for SX1276/RFM95W on LilyGO TTGO LoRa32 T3 v1.6.1 and similar
ESP32 boards.
"""

from esphome import pins
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID

CODEOWNERS = ["@magicx78"]
DEPENDENCIES = ["esp32"]
MULTI_CONF = False

bresser_weather_ns = cg.esphome_ns.namespace("bresser_weather")
BresserWeatherHub = bresser_weather_ns.class_("BresserWeatherHub", cg.Component)
BresserField = bresser_weather_ns.enum("BresserField")

CONF_SCK_PIN = "sck_pin"
CONF_MISO_PIN = "miso_pin"
CONF_MOSI_PIN = "mosi_pin"
CONF_CS_PIN = "cs_pin"
CONF_RST_PIN = "rst_pin"
CONF_DIO0_PIN = "dio0_pin"
CONF_DIO1_PIN = "dio1_pin"
CONF_DECODERS = "decoders"
CONF_MAX_SENSORS = "max_sensors"

DECODER_FLAGS = {
    "5in1": "BRESSER_5_IN_1",
    "6in1": "BRESSER_6_IN_1",
    "7in1": "BRESSER_7_IN_1",
}

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(BresserWeatherHub),
        cv.Required(CONF_SCK_PIN): pins.internal_gpio_output_pin_number,
        cv.Required(CONF_MISO_PIN): pins.internal_gpio_input_pin_number,
        cv.Required(CONF_MOSI_PIN): pins.internal_gpio_output_pin_number,
        cv.Required(CONF_CS_PIN): pins.internal_gpio_output_pin_number,
        cv.Required(CONF_RST_PIN): pins.internal_gpio_output_pin_number,
        cv.Required(CONF_DIO0_PIN): pins.internal_gpio_input_pin_number,
        cv.Required(CONF_DIO1_PIN): pins.internal_gpio_input_pin_number,
        cv.Optional(CONF_DECODERS, default=list(DECODER_FLAGS.keys())): cv.ensure_list(
            cv.one_of(*DECODER_FLAGS.keys(), lower=True)
        ),
        cv.Optional(CONF_MAX_SENSORS, default=8): cv.int_range(min=1, max=32),
    }
).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    cg.add(var.set_sck_pin(config[CONF_SCK_PIN]))
    cg.add(var.set_miso_pin(config[CONF_MISO_PIN]))
    cg.add(var.set_mosi_pin(config[CONF_MOSI_PIN]))
    cg.add(var.set_cs_pin(config[CONF_CS_PIN]))

    cg.add_build_flag(f"-DPIN_RECEIVER_CS={config[CONF_CS_PIN]}")
    cg.add_build_flag(f"-DPIN_RECEIVER_IRQ={config[CONF_DIO0_PIN]}")
    cg.add_build_flag(f"-DPIN_RECEIVER_GPIO={config[CONF_DIO1_PIN]}")
    cg.add_build_flag(f"-DPIN_RECEIVER_RST={config[CONF_RST_PIN]}")
    cg.add_build_flag("-DUSE_SX1276")
    cg.add_build_flag(f"-DMAX_SENSORS_DEFAULT={config[CONF_MAX_SENSORS]}")
    for decoder in config[CONF_DECODERS]:
        cg.add_build_flag(f"-D{DECODER_FLAGS[decoder]}")

    # RadioLib version pinned to match upstream library.properties (v0.41.0 -> RadioLib 7.6.0)
    cg.add_library("RadioLib", "7.6.0")
