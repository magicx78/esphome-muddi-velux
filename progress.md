# muddi-velux – Fortschritt & Fehlerbehebung

## Gerät
- **Name:** muddi-velux
- **Board:** ESP32 (esp32dev)
- **Fenster:** 6x Velux-Motoren via PCF8574 I/O-Expander (Relais)
- **HA-Anbindung:** ESPHome Native API

## Fenster-Belegung
| Kanal | Name | Öffnen (GPIO/PCF) | Schließen | Dauer |
|-------|------|-------------------|-----------|-------|
| 1 | Gäste Schlafzimmer | outputs_1_8 pin 0 | pin 1 | 29s / 29s |
| 2 | Küchen Velux Fenster | outputs_1_8 pin 2 | pin 3 | 23s / 24s |
| 3 | **Empore** | outputs_1_8 pin 4 | pin 5 | 30s / 30s |
| 4 | Wohnzimmer Velux Fenster | outputs_1_8 pin 6 | pin 7 | 33s / 28s |
| 5 | Schlafzimmer Links | outputs_9_16 pin 0 | pin 1 | 30s / 30s |
| 6 | Schlafzimmer Rechts | outputs_9_16 pin 2 | pin 3 | 34s / 34s |

## Behobene Fehler

### 2026-04-15

| # | Fehler | Symptom | Fix |
|---|--------|---------|-----|
| 1 | Falsche Datei kopiert (velux-new.yaml, 687 Zeilen, unvollständig) | Kompiliert nicht — globals/switch/cover/script fehlten | Durch vollständige muddi-velux.yaml (1218 Zeilen) ersetzt |
| 2 | `latitude: 52,0302` — Komma statt Punkt | Sonnen-Tracking defekt → Empore öffnet/schließt zur falschen Zeit oder gar nicht | Auf `latitude: 52.0302` korrigiert |
| 3 | `longitude: 8,5325` — Komma statt Punkt | Selbe Auswirkung wie #2 | Auf `longitude: 8.5325` korrigiert |
| 4 | `sntp_time` doppelt definiert (inline + components/monitoring.yaml) | Compile-Fehler: ID mehrfach vergeben | Inline `time:`-Block entfernt — kommt jetzt aus monitoring.yaml |
| 5 | WiFi-Passwort hardcoded im YAML | Sicherheitsproblem | Auf `!secret wifi_ssid` / `!secret wifi_password` umgestellt |

## Empore (Fenster 3) — Diagnose
- Hauptursache der Fehlfunktion: **Komma statt Punkt in lat/lon** → `sun`-Komponente berechnete falsche Sonnenuntergangszeiten → automatisches Öffnen/Schließen defekt
- Relais: PCF8574 outputs_1_8, Pins 4 (auf) und 5 (zu), interlock aktiviert
- Fahrdauer: 30s auf, 30s zu (bei Problemen ggf. anpassen)

## Abhängigkeiten
| Datei | Zweck |
|-------|-------|
| `muddi-velux.yaml` | Hauptkonfiguration |
| `components/monitoring.yaml` | API-Status, SNTP-Zeit, Uptime, Diagnose-Sensoren |

## Offene Punkte
- [ ] Fahrdauer Empore (30s) verifizieren — ggf. anpassen wenn Fenster nicht vollständig öffnet/schließt
- [ ] PCF8574-Adressen verifizieren (inputs: 0x22, 0x21 / outputs: 0x24, 0x25)
