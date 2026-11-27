# Wiring

> Pin numbers on the car side are **not verified by this project yet**. Check them against your harness documentation and confirm with the serial sniffer before trusting any data.

## ESP32 ↔ SN65HVD230

| SN65HVD230 module | ESP32 DevKitC | ESP32-C6 DevKitC-1 |
|---|---|---|
| 3V3 | 3V3 | 3V3 |
| GND | GND | GND |
| CTX / D (TX) | GPIO 5 (`OTB_CAN_TX_PIN`) | GPIO 5 (`OTB_CAN_TX_PIN`) |
| CRX / R (RX) | GPIO 4 (`OTB_CAN_RX_PIN`) | GPIO 4 (`OTB_CAN_RX_PIN`) |
| CANH | → car CAN-H | |
| CANL | → car CAN-L | |

The GPIOs are defaults set in `platformio.ini` and can be changed freely. The TX line is wired because the TWAI driver requires a TX pin, but in listen-only mode the controller never drives it.

**Termination:** many SN65HVD230 modules have an onboard 120 Ω resistor. The car's bus is already terminated at both ends. For a short-stub listen-only tap it usually works either way, but removing the extra resistor is the cleaner option.

## Car side: Model 3

The Model 3 has **no OBD2 port in the footwell**. The diagnostic connector sits **behind the center console**, in the rear footwell area under the rear air vents. A diagnostic harness (sold as "Model 3 diagnostic cable" or "Kabelsatz M3") plugs in there and exposes a standard OBD2 socket.

Which CAN bus ends up on which OBD2 pins depends on the harness. Many harnesses put a CAN pair on OBD2 pins 6 (CAN-H) and 14 (CAN-L), but **you need the pair that carries Vehicle CAN**. The battery frames (0x292, 0x352) are only on Vehicle CAN.

| Item | Value | Status |
|---|---|---|
| Bus | Vehicle CAN | from brief / community docs |
| Bitrate | 500 kbit/s | from brief / community docs |
| Harness pins for Vehicle CAN | TBD | to be documented after test on reference car |
| Diagnostic connector pinout | TBD | to be documented with photos |

## Checking you are on the right bus

1. Build with `-DOTB_SERIAL_SNIFFER=1` and flash.
2. Capture: `pio device monitor | tee capture.log` for ~30 seconds with the car awake.
3. Run `python3 tools/can_log_summary.py capture.log`.
4. If 0x292 and 0x352 are missing, you are on the wrong bus or pins. This is the most common failure.

If `rx` stays at 0 and `buserr` grows, check CAN-H/CAN-L polarity and the bitrate.
