# CAN mapping

All signals below are on the **Vehicle CAN** bus at **500 kbit/s**, 11-bit IDs, little-endian (Intel, DBC `@1`).

Source: [joshwardell/model3dbc](https://github.com/joshwardell/model3dbc), `Model3CAN.dbc`, commit `c56c113`. These definitions are community reverse-engineered. **Validation status** says whether this project has checked the value against a reference tool on a real car.

Notation: `start|length (scale, offset) unit`, as in the DBC.

## Used by the MVP

### 0x292 `ID292BMS_SOC`

| Signal | Layout | Meaning | SNA | Decoded | Validated |
|---|---|---|---|---|---|
| `BattBeginningOfLifeEnergy292` | `40\|10 (0.1,0) kWh` | Original pack energy | 1023 | yes | no |
| `SOCUI292` | `10\|10 (0.1,0) %` | SOC shown in UI | – | yes | no |
| `SOCmin292` | `0\|10 (0.1,0) %` | | – | no | no |
| `SOCmax292` | `20\|10 (0.1,0) %` | | – | no | no |
| `SOCave292` | `30\|10 (0.1,0) %` | | – | no | no |
| `BMS_battTempPct` | `50\|8 (0.4,0) %` | | 255 | no | no |

### 0x352 `ID352BMS_energyStatus`

| Signal | Layout | Meaning | SNA | Decoded | Validated |
|---|---|---|---|---|---|
| `BMS_nominalFullPackEnergy` | `0\|11 (0.1,0) kWh` | Current full pack energy | 2047 | yes | no |
| `BMS_nominalEnergyRemaining` | `11\|11 (0.1,0) kWh` | Energy remaining | 2047 | yes | no |
| `BMS_expectedEnergyRemaining` | `22\|11 (0.1,0) kWh` | | 2047 | no | no |
| `BMS_idealEnergyRemaining` | `33\|11 (0.1,0) kWh` | | 2047 | no | no |
| `BMS_energyToChargeComplete` | `44\|11 (0.1,0) kWh` | | 2047 | no | no |
| `BMS_energyBuffer` | `55\|8 (0.1,0) kWh` | Bottom buffer | 255 | yes | no |
| `BMS_fullChargeComplete` | `63\|1` | | – | no | no |

> **Known risk (unconfirmed):** community reports suggest the 0x352 layout changed (multiplexed) on some newer car firmware. If decoded values look wrong, capture raw frames and open a validation report.

## Planned (v0.2)

| ID | DBC message | Signals | Use |
|---|---|---|---|
| 0x132 | `ID132HVBattAmpVolt` | `BattVoltage132 0\|16 (0.01,0) V`, `SmoothBattCurrent132 16\|16 signed (-0.1,0) A`, `RawBattCurrent132 32\|16 signed (-0.05,822) A` | Live pack telemetry |
| 0x332 | `ID332BattBrickMinMax` (mux `0\|2`) | m1: `BattBrickVoltageMax332 2\|12 (0.002,0) V`, `BattBrickVoltageMin332 16\|12 (0.002,0) V`; m0: brick temperatures `(0.5,-40) °C` | Spread, temperatures |
| 0x401 | `ID401BrickVoltages` (mux `0\|8`) | `BrickN 16\|16, 32\|16, 48\|16 (0.0001,0) V`, 3 bricks per mux value | Per-group voltages |
| 0x33A | `ID33AUI_rangeSOC` | `UI_ratedWHpM 32\|10 (1,0) Wh/mi`, `UI_Range 0\|10 mi` | Range at 100% |
| 0x3D2 | `ID3D2TotalChargeDischarge` | `TotalDischargeKWh3D2 0\|32 (0.001,0) kWh`, `TotalChargeKWh3D2 32\|32 (0.001,0) kWh` | Equivalent cycles |

Note: the RawBattCurrent132 DBC comment says "old offset 1000", a hint that offsets have changed over time. Always cross-check.

## Adding a signal

1. Copy the `SG_` line from the DBC into a comment next to the decoder.
2. Add a unit test with a synthetic payload.
3. Mark it **not validated** here until a [car validation report](https://github.com/lapht/OpenTeslaBatteryESP/issues/new?template=car_validation.yml) confirms it.
