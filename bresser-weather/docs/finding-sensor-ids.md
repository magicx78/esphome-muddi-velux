# Finding your Bresser sensor IDs

Each Bresser transmitter has a hardware ID that changes on every battery
swap. On first flash you don't yet know which IDs are in range — use the
discovery workflow below.

## Workflow

1. Copy `example.yaml` to your local config, edit the WiFi block.
2. Delete every `sensor:` entry under the `bresser_weather` platform so
   the file has just the `bresser_weather:` hub block.
3. Set `logger: level: DEBUG`.
4. `esphome run yourconfig.yaml --device /dev/ttyUSB0` (or use the
   ESPHome dashboard).
5. Watch the serial log. Within ~60 seconds you should see lines like:

   ```
   [D][bresser_weather] RX id=0x12345678 type=1 chan=0 dec=2 rssi=-78.5 batt_ok=1
   [D][bresser_weather]   temp=21.3°C
   [D][bresser_weather]   humidity=58%
   ```

   `type` is the Bresser sensor type: 0/1 = weather station, 4 = soil,
   5 = leakage, 9 = lightning or pro rain gauge, etc.
6. Pick the IDs you care about and add `sensor:` entries to your YAML:

   ```yaml
   sensor:
     - platform: bresser_weather
       sensor_id: 0x12345678
       temperature: { name: "Garden Temperature" }
   ```

7. Re-flash. The new entities appear in Home Assistant via the ESPHome API.

## When you swap batteries

The sensor_id changes. Re-run the discovery step (delete `sensor:`,
re-flash, read log) and update your YAML with the new IDs.

## Filtering noise

If multiple neighbours' Bresser stations are in range you will see many
distinct IDs. The component already filters to the ones declared in
`sensor:` for state publication, but the discovery log shows every packet.
Once your IDs are stable, lower `logger: level: INFO` to quiet things down.
