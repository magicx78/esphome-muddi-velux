// SPDX-License-Identifier: BSD-2-Clause
//
// WeatherSensorCfg.h — ESPHome shim for BresserWeatherSensorReceiver.
//
// This file replaces upstream's src/WeatherSensorCfg.h. It strips all
// Arduino board-detection logic and consumes pin/option values that the
// ESPHome Python codegen injects as -D defines.
//
// Required defines (must be set via build flags from __init__.py):
//   PIN_RECEIVER_CS, PIN_RECEIVER_IRQ, PIN_RECEIVER_GPIO, PIN_RECEIVER_RST
//   USE_SX1276            -> selects RadioLib SX1276 (only chip supported in MVP)
//
// Optional defines:
//   BRESSER_5_IN_1, BRESSER_6_IN_1, BRESSER_7_IN_1  -> decoders (default: all on)
//   BRESSER_LIGHTNING, BRESSER_LEAKAGE              -> deferred, default off
//   MAX_SENSORS_DEFAULT   -> default 8
//
// Upstream reference: BresserWeatherSensorReceiver v0.41.0
//   https://github.com/matthias-bs/BresserWeatherSensorReceiver/blob/v0.41.0/src/WeatherSensorCfg.h

#if !defined(WEATHER_SENSOR_CFG_H)
#define WEATHER_SENSOR_CFG_H

#include <Arduino.h>

#if !defined(PIN_RECEIVER_CS) || !defined(PIN_RECEIVER_IRQ) || \
    !defined(PIN_RECEIVER_GPIO) || !defined(PIN_RECEIVER_RST)
#  error "Bresser shim: PIN_RECEIVER_{CS,IRQ,GPIO,RST} must be defined via build flags (see ESPHome __init__.py)"
#endif

#if !defined(USE_SX1276) && !defined(USE_SX1262) && !defined(USE_CC1101) && !defined(USE_LR1121)
#  define USE_SX1276
#endif

#if !defined(MAX_SENSORS_DEFAULT)
#  define MAX_SENSORS_DEFAULT 8
#endif

#define SENSOR_IDS_EXC { }
#define SENSOR_IDS_INC { }
#define MAX_SENSOR_IDS 16

#define WIND_DATA_FLOATINGPOINT
#define WIND_DATA_FIXEDPOINT

#if !defined(BRESSER_5_IN_1) && !defined(BRESSER_6_IN_1) && !defined(BRESSER_7_IN_1) && \
    !defined(BRESSER_LIGHTNING) && !defined(BRESSER_LEAKAGE)
#  define BRESSER_5_IN_1
#  define BRESSER_6_IN_1
#  define BRESSER_7_IN_1
#endif

// Persist rain/lightning counters across reboots via NVS Preferences.
// Off by default — ESPHome typically runs continuously and frequent NVS writes
// from a radio ISR path are undesirable. Enable explicitly if you need it.
// #define RAINGAUGE_USE_PREFS
// #define LIGHTNING_USE_PREFS

#if defined(USE_CC1101)
#  define RADIO_CHIP CC1101
#elif defined(USE_SX1276)
#  define RADIO_CHIP SX1276
#elif defined(USE_SX1262)
#  define RADIO_CHIP SX1262
#elif defined(USE_LR1121)
#  define RADIO_CHIP LR1121
#endif

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)
#define RECEIVER_CHIP "[" STR(RADIO_CHIP) "]"

#endif  // WEATHER_SENSOR_CFG_H
