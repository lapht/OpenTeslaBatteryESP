# Safety and legal

## Read-only by design

- The TWAI controller runs in `TWAI_MODE_LISTEN_ONLY`: it sends no frames, no ACK bits and no error frames.
- The code has no transmit function. `CanReader` exposes `begin`, `read` and `end` only.
- Contributions that add transmission, injection, "unlocks" or configuration changes are out of scope and will be rejected.

Because nothing is sent, the device does not alter the car's configuration. It still physically connects to the car's wiring: do it carefully, with the car parked.

## Electrical

- Use a 3.3 V transceiver (SN65HVD230) or make sure logic levels match the ESP32.
- Do not short CAN-H/CAN-L to 12 V or ground.
- The diagnostic port can keep the car awake. **Unplug the device after use** to avoid draining the 12 V battery.
- Do not leave a bare dev board loose in the car: use an enclosure.

## Data accuracy

Signal definitions come from community reverse engineering. Values may be wrong, may change with car software updates, and are **not** an official Tesla diagnosis. Do not base a purchase, warranty claim or safety decision solely on them.

## Trademarks and affiliation

This is an independent open-source project, not affiliated with, endorsed or sponsored by Tesla, Inc. Tesla, Model 3, Model Y, Model S and Model X are trademarks of Tesla, Inc.

## Warranty

The software is provided "as is", without warranty of any kind (see [LICENSE](../LICENSE)). You use it at your own risk.

## Privacy

The firmware does not connect to the internet. Data stays on the device and is served only on its local access point. Note that the CAN bus may carry identifying data (such as the VIN): remove it before sharing logs.
