# Battery health

## Formulas

| Figure | Formula | Inputs | Status |
|---|---|---|---|
| State of health (SOH) | `nominalFullPackEnergy / beginningOfLifeEnergy × 100` | 0x352, 0x292 | MVP |
| Degradation | `100 − SOH` | | MVP |
| Range at 100 % | `(nominalFullPackEnergy − energyBuffer) / ratedWhPerMile` | 0x352, 0x33A | v0.2 |
| Equivalent full cycles | `lifetimeDischargeKWh / beginningOfLifeEnergy` | 0x3D2, 0x292 | v0.2 |
| Brick spread | `maxBrickV − minBrickV` | 0x332 / 0x401 | v0.2 |

Open question for the MVP: whether SOH should subtract the energy buffer from both numerator and denominator. The first version follows the plain ratio above; this will be revisited after comparing with reference tools.

## What this is not

This is the BMS's own estimate of pack energy, read off the bus. It is **not** Tesla's official battery health test, and it is not a lab capacity measurement.

## Known limitations

1. **SOH drifts.** The BMS recalibrates its capacity estimate after deep discharges and full charges, so the value can move a few percent from day to day. The most stable reading comes after a charge to 100 % followed by a rest period.
2. **Community scaling can be wrong.** Some DBC signals have had wrong offsets in the past. Values are only trusted after validation against a reference tool (see [validation.md](validation.md)).
3. **LFP packs.** On LFP chemistry the cell voltage curve is flat across most of the SOC range, so voltage-based estimates are unreliable. The reference car (Model 3 LR AWD 2021) is NCA. LFP detection and a warning are planned for v0.3.
4. **Firmware changes.** Tesla can change frame layouts with software updates.
