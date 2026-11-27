# Hardware

## Bill of materials

| Part | Example | Notes |
|---|---|---|
| ESP32 board | ESP32 DevKitC (WROOM-32) | Primary target. Native TWAI controller. |
| or | ESP32-C6 DevKitC-1 | Secondary target, also has TWAI. |
| CAN transceiver | SN65HVD230 module | 3.3 V, connects directly to the ESP. MCP2551 is 5 V and needs level care; MCP2562 has a separate logic supply (VIO) and works with 3.3 V logic. |
| Diagnostic harness | Model 3 diagnostic cable with OBD2 socket | Must expose the **Vehicle CAN** pair. See [wiring.md](wiring.md). |
| Power | 12 V → 5 V buck converter, or USB from the car | Mind the 12 V drain, see below. |
| Misc | Dupont wires, enclosure | |

Prices are not listed on purpose: they change. Rough total is in the "tens of euros" range, not hundreds.

## Reference setup

The project is developed and validated on:

- **Car:** Tesla Model 3 Long Range AWD, 2021, HW3, NCA chemistry
- **Board:** ESP32 DevKitC
- **Transceiver:** SN65HVD230

Other combinations are welcome; please file a [car validation report](https://github.com/lapht/OpenTeslaBatteryESP/issues/new?template=car_validation.yml).

## 12 V battery drain

The diagnostic port is powered and an active device on the bus can keep the car's systems awake. An ESP left plugged in permanently will drain the 12 V battery. Until power management lands (v0.4), **unplug the device after use**.

## Future

A dedicated PCB (ESP32 + transceiver + Tesla connector in one module) is on the roadmap. Design files will live in [`hardware/`](../hardware/).
