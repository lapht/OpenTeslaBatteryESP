# Architecture

## Overview

```
┌────────────┐   ┌──────────────┐   ┌────────────────────────── ESP32 ───────────────────────────┐
│ Vehicle CAN│──►│ SN65HVD230   │──►│ TWAI (listen-only) ─► canTask ─► BmsDecoder ─► BmsState   │
│ 500 kbit/s │   │ transceiver  │   │                                                   │         │
└────────────┘   └──────────────┘   │                          HealthCalculator ◄───────┘         │
                                    │                                 │                           │
                                    │  WiFi SoftAP ◄── AsyncWebServer: /  and  /api/status        │
                                    │      └─ DNS :53 (tesla.lan, captive) + mDNS (tesla.local)   │
                                    └─────────────────────────────────────────────────────────────┘
```

## Source layout

```
src/
  main.cpp                 setup, CAN task, wiring of modules
  config.h                 build-time settings (pins, SSID, timeouts)
  can/CanReader.*          TWAI driver wrapper, listen-only, no TX API
  bms/Signal.h             DBC little-endian signal extraction
  bms/BmsDecoder.*         CAN ID -> physical values (BmsState)
  bms/HealthCalculator.*   SOH and derived figures
  web/WebUi.*              SoftAP, HTTP server, JSON API, DNS/captive portal, mDNS
data/index.html            single-file mobile web page (LittleFS)
test/                      host-side unit tests (pio test -e native)
tools/                     host scripts (log analysis)
docs/                      documentation
hardware/                  hardware notes, future PCB
```

## Design decisions

| Decision | Choice | Why |
|---|---|---|
| Framework | Arduino-ESP32 core 3.x on PlatformIO (pioarduino platform) | Fast prototyping, ESPAsyncWebServer, ESP32-C6 support. The IDF TWAI driver is used directly. |
| CAN mode | `TWAI_MODE_LISTEN_ONLY` | Hard project rule: zero bus interaction. |
| Concurrency | Dedicated FreeRTOS task for CAN RX, state guarded by a spinlock | Keeps the web server from blocking frame reception. |
| Decoding | Hand-written decoders citing DBC lines | Small footprint, easy to audit. A DBC-driven generator may come later. |
| UI | Static HTML + JSON polling every 2 s | Simple and robust. WebSocket push is planned for v0.2. |
| Friendly URL | Own DNS server on the SoftAP (wildcard, captive portal) + mDNS | Works on every OS without setup. See [web-access.md](web-access.md). |
| Storage | LittleFS for the web page | Page can be edited without recompiling the firmware. |

## Data flow and staleness

Each decoded value carries `valid` and `lastSeenMs`. Values equal to the DBC "SNA" (signal not available) marker are stored as invalid. Health is computed only when both inputs were seen within `OTB_STALE_MS` (default 5 s).

## HTTP API

`GET /api/status`

```json
{
  "health": { "valid": true, "soh_pct": 90.0, "degradation_pct": 10.0, "original_kwh": 80.0, "current_kwh": 72.0 },
  "bms": { "beginning_of_life_kwh": 80.0, "nominal_full_pack_kwh": 72.0, "nominal_remaining_kwh": 40.0, "energy_buffer_kwh": 3.2, "soc_ui_pct": 50.0 },
  "bms_age_ms": { "beginning_of_life_kwh": 180, "nominal_full_pack_kwh": 90, "nominal_remaining_kwh": 90, "energy_buffer_kwh": 90, "soc_ui_pct": 180 },
  "can": { "received": 12345, "missed": 0, "bus_errors": 0, "last_frame_age_ms": 3,
           "battery_frames": 300, "last_battery_frame_age_ms": 90 },
  "link": "ok",
  "stale_ms": 5000,
  "uptime_ms": 60000
}
```

Example numbers above are illustrative, not real car data. Fields that were never received (or whose last value was SNA) are `null`, and so is their age.

- `bms_age_ms`: milliseconds since each value was last updated. The page greys out values older than `stale_ms` and shows how old they are.
- `link`: `ok` (battery frames within `stale_ms`), `no_battery_frames` (CAN traffic but no 0x292/0x352: probably the wrong bus), `no_traffic` (nothing received: wiring, or the car is asleep). "Device unreachable" is shown by the page when the request itself fails.

## Hardware filters

Accept-all is used for now so the serial sniffer can show the whole bus. Once the battery ID set is final, the TWAI acceptance filter will be narrowed to reduce CPU load (tracked in the issue tracker).
