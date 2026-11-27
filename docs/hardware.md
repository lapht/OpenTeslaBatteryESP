# Hardware

## Shopping list

Search terms work on AliExpress, Amazon or eBay. No links or prices on purpose: listings change constantly. Check the notes column before buying.

### Core (needed for the first test)

| # | Part | Search for | Notes |
|---|---|---|---|
| 1 | ESP32 board | `ESP32 DevKitC V4 WROOM-32` (USB-C variant if you like) | Primary target. Alternative: `ESP32-C6-DevKitC-1`. Avoid ESP32-S2 (no TWAI issue, but not a build target) and ESP32-C3 "super mini" boards unless you change `platformio.ini`. |
| 2 | CAN transceiver | `SN65HVD230 CAN module` (often sold as "VP230") | 3.3 V. Check whether it has an onboard 120 Ω resistor (usually marked R2 or "120"): you will want to remove it. Buy 2, they are cheap and easy to kill. |
| 3 | OBD-II plug with wires | `OBD2 16 pin male connector pigtail` / `OBD2 male plug open end cable` | Must be **male** (plugs into the car). Pigtail with all 16 wires and a printed colour/pin table is easiest. |
| 4 | Jumper wires | `dupont jumper wire female female` | For bench wiring. |
| 5 | USB power bank + USB cable | – | Powers the ESP for the first test without touching the car's 12 V. |

### For permanent install (after the first test works)

| # | Part | Search for | Notes |
|---|---|---|---|
| 6 | 12 V → 5 V converter | `DC-DC buck 12V to 5V 3A` (fixed-output module, e.g. "Mini560 5V") | Input rating must cover at least 9–24 V. Fixed 5 V output avoids a trimmer that can drift. |
| 7 | Inline fuse | `inline fuse holder mini blade` + `1A blade fuse` | On the +12 V wire, close to the OBD plug. |
| 8 | TVS diode (optional, recommended) | `SMBJ18A` or `P6KE18A` | Across +12 V / GND at the buck input, protects against voltage spikes. |
| 9 | Perfboard or small PCB, headers | `perfboard 5x7`, `pin header female 2.54` | To replace the jumper wires. |
| 10 | Enclosure | `ABS project box 80x50x25` | Anything that fits board + transceiver + buck. |

### Only if Option A does not work (see [wiring.md](wiring.md))

| # | Part | Search for | Notes |
|---|---|---|---|
| 11 | Center console adapter | `Tesla Model 3 Y 26 pin OBD2 adapter cable` | For Model 3 2019+ / Model Y. Model 3 built before 2019 uses a **20-pin** version. Check that the listing says which bus it maps to OBD-II pins 6/14. |

### Tools

Soldering iron, multimeter (to check pinouts before plugging anything in), USB cable for flashing.

## Reference setup

The project is developed and validated on:

- **Car:** Tesla Model 3 Long Range AWD, 2021, HW3, NCA chemistry
- **Board:** ESP32 DevKitC
- **Transceiver:** SN65HVD230

Other combinations are welcome; please file a [car validation report](https://github.com/lapht/OpenTeslaBatteryESP/issues/new?template=car_validation.yml).

## 12 V battery drain

According to Tesla service bulletin CD-20-17-001, the +12 V on pin 16 of the OBD-II port X181 is only powered **while the car is awake** (1 A max on 2019 to April 2024 cars), so a device on that port is switched off when the car sleeps. Whether the device itself can prevent the car from going to sleep has not been tested yet: until then, **unplug it after use**. The power pin on the center console connector (Option B) has not been characterised.

## Future

A dedicated PCB (ESP32 + transceiver + Tesla connector in one module) is on the roadmap. Design files will live in [`hardware/`](../hardware/).
