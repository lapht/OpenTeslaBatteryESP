# Validation

No value is considered correct until it has been compared with an independent reference **at the same moment, on the same car**.

## Procedure

1. Car awake, parked, ideally after a charge to 100 % and some rest (most stable BMS estimate).
2. Connect OpenTeslaBatteryESP and a reference tool (e.g. Scan My Tesla) at the same time, or take readings back-to-back within a minute.
3. Record for each signal: our value, reference value, tool name, car software version.
4. Optionally capture raw frames with `OTB_SERIAL_SNIFFER=1`.
5. File a [car validation report](https://github.com/lapht/OpenTeslaBatteryESP/issues/new?template=car_validation.yml).

## Acceptance

A signal is marked **validated** in [can-mapping.md](can-mapping.md) when it matches the reference within its DBC resolution (e.g. 0.1 kWh) on at least one car. Mismatches are documented with the raw frames so the scaling can be fixed.

## Validation log

| Date | Car | Software | Signal | Result | Report |
|---|---|---|---|---|---|
| – | – | – | – | none yet | – |
