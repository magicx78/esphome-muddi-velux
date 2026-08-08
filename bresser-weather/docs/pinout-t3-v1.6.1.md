# LilyGO TTGO LoRa32 T3 v1.6.1 pinout

| Function | ESP32 GPIO | Notes |
|---|---|---|
| SX1276 SCK | 5 | SPI clock |
| SX1276 MISO | 19 | |
| SX1276 MOSI | 27 | |
| SX1276 CS (NSS) | 18 | |
| SX1276 RST | 23 | |
| SX1276 DIO0 | 26 | RX-done IRQ in FSK |
| SX1276 DIO1 | 33 | FIFO level — required by RadioLib for FSK |
| SX1276 DIO2 | 32 | unused by this component (shared with SX1262 BUSY) |
| On-board LED | 25 | active high (`LED_ON = HIGH`) |
| Boot / user button | 0 | strapping pin |
| Battery ADC | 35 | input-only, 100k/100k divider, max 4.2 V |
| OLED SDA | 21 | shared I2C bus (SSD1306 128x64) |
| OLED SCL | 22 | |
| OLED RST | (unused) | LilyGO `OLED_RST = UNUSED_PIN` |
| SD card MOSI | 15 | only on board variants with SD slot |
| SD card MISO | 2 | |
| SD card SCLK | 14 | |
| SD card CS | 13 | |

Source: LilyGO's official
[`examples/RadioLibExamples/.../utilities.h`](https://github.com/Xinyuan-LilyGO/LilyGo-LoRa-Series/blob/master/examples/RadioLibExamples/Receive_Interrupt/utilities.h)
in the `T3_V1_6_SX1276` section. The 433 MHz sister variant
`T3_V1_6_SX1278` uses the same pin numbers but is unsuitable for 868 MHz
Bresser sensors — verify your board's radio chip part before flashing.

These pin numbers are reflected in `../example.yaml`. The `ttgo-lora32-v21new`
board variant in `arduino-esp32` also defines them as `LORA_*` macros, but
this component sets them explicitly so you can move to a board without those
macros and still get the same wiring.

## Verifying the board

Hold the boot button while plugging in USB → release. The on-board LED on
GPIO 25 should pulse during ESPHome flash. If you see a board version sticker
that reads `T3_v1.6.1`, the table above applies. Other T3 revisions (v1.0,
v2.0, v2.1) move some pins around — consult the LilyGO board photo guide.
