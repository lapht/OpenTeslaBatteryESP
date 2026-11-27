# Roadmap

Tracked as [GitHub milestones](https://github.com/lapht/OpenTeslaBatteryESP/milestones). This page is the summary.

| Milestone | Goal |
|---|---|
| **v0.1 MVP** | Listen-only 500k, decode 0x292/0x352, SOH + degradation, web page with SOH and capacity. Validated on Model 3 LR AWD 2021. |
| **v0.2 Telemetry** | Brick voltages and spread, pack voltage/current, range at 100 %, equivalent cycles, live charts over WebSocket. |
| **v0.3 Vehicle info** | Gateway config (hardware, pack, drivetrain), basic BMS alert decoding, LFP detection with warning. |
| **v0.4 Power and history** | Deep sleep / bus-idle detection, 12 V drain measurement, SOH history on flash or SD, optional STA mode. |
| **v1.0** | Model Y support, Model 3 2017–2024 coverage, complete hardware docs, showcase site with browser flashing (ESP Web Tools). |
| **Future** | Dedicated PCB, prebuilt release binaries. |

Not on the roadmap, ever: anything that transmits on the bus.
