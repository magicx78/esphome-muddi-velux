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
| SX1276 DIO2 | 32 | unused by this component |
| On-board LED | 25 | active high |
| Boot / user button | 0 | strapping pin |
| Battery ADC | 35 | input-only, voltage divider |
| OLED SDA (if fitted) | 21 | not used here |
| OLED SCL (if fitted) | 22 | not used here |
| OLED RST (if fitted) | 16 | |

These pin numbers are reflected in `../example.yaml`. The `ttgo-lora32-v21new`
board variant in `arduino-esp32` also defines them as `LORA_*` macros, but
this component sets them explicitly so you can move to a board without those
macros and still get the same wiring.

## Verifying the board

Hold the boot button while plugging in USB → release. The on-board LED on
GPIO 25 should pulse during ESPHome flash. If you see a board version sticker
that reads `T3_v1.6.1`, the table above applies. Other T3 revisions (v1.0,
v2.0, v2.1) move some pins around — consult the LilyGO board photo guide.
