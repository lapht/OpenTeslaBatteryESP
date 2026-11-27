# Battery health

## Formulas

| Figure | Formula | Inputs | Status |
|---|---|---|---|
| State of health (SOH) | `nominalFullPackEnergy / beginningOfLifeEnergy × 100` | 0x352, 0x292 | MVP |
| Degradation | `100 − SOH` | | MVP |
| Range at 100 % | `(nominalFullPackEnergy − energyBuffer) / ratedWhPerMile` | 0x352, 0x33A | v0.2 |
| Equivalent full cycles | `lifetimeDischargeKWh / beginningOfLifeEnergy` | 0x3D2, 0x292 | v0.2 |
| Brick spread | `maxBrickV − minBrickV` | 0x332 / 0x401 | v0.2 |

## SOH and the energy buffer (decision for v0.1)

SOH is the **plain ratio** `nominalFullPackEnergy / beginningOfLifeEnergy`, with the buffer **not** subtracted.

Why:

- Both numbers describe the whole pack. `BMS_nominalFullPackEnergy` includes the bottom buffer (`BMS_energyBuffer` is reported separately and, per common community usage, is part of it; to be confirmed in #5), and `BattBeginningOfLifeEnergy` is the full-pack energy when new. Same scope on both sides, so the ratio compares like with like.
- Subtracting the buffer would need the buffer *at beginning of life*, which the car does not report. Subtracting today's buffer from both sides mixes a current value into the original figure, and since the buffer is a few kWh that skews the result by a percent or more.
- The plain ratio uses two values only, so there is less to get wrong while the signals are still unvalidated.

What the page shows besides SOH: the buffer and the energy remaining, as raw values, so anyone who prefers a "usable capacity" view (`nominalFullPack − buffer`) can do the math.

This is the BMS's own view, so it may differ from what other tools show if they use a different formula or different inputs. The numbers will be compared with Scan My Tesla on the reference car as part of the 0x292/0x352 validation ([#4](https://github.com/lapht/OpenTeslaBatteryESP/issues/4), [#5](https://github.com/lapht/OpenTeslaBatteryESP/issues/5)); if they disagree in a systematic way, the formula is revisited there.

## What this is not

This is the BMS's own estimate of pack energy, read off the bus. It is **not** Tesla's official battery health test, and it is not a lab capacity measurement.

## Known limitations

1. **SOH drifts.** The BMS recalibrates its capacity estimate after deep discharges and full charges, so the value can move a few percent from day to day. The most stable reading comes after a charge to 100 % followed by a rest period.
2. **Community scaling can be wrong.** Some DBC signals have had wrong offsets in the past. Values are only trusted after validation against a reference tool (see [validation.md](validation.md)).
3. **LFP packs.** On LFP chemistry the cell voltage curve is flat across most of the SOC range, so voltage-based estimates are unreliable. The reference car (Model 3 LR AWD 2021) is NCA. LFP detection and a warning are planned for v0.3.
4. **Firmware changes.** Tesla can change frame layouts with software updates.
