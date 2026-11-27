# Security

## Supported versions

Only the latest commit on `main` (and, once published, the latest release) receives fixes.

## Scope

Relevant reports include anything that could:

- make the device **transmit on the CAN bus** or bypass listen-only mode;
- expose the web interface or the DNS server beyond the local access point;
- let a client on the access point change the firmware, the configuration or the stored data;
- leak data from the car (for example the VIN) beyond what the page shows.

## Reporting

Please do not open a public issue. Use GitHub's [private vulnerability reporting](https://github.com/lapht/OpenTeslaBatteryESP/security/advisories/new) for this repository. You should get an answer within a week.

## Threat model and known limitations

The device is a local-only access point with no internet uplink, no OTA update and no write API.

- Each device generates a random SoftAP password on first boot (hardware RNG, ~59 bits, stored in NVS) and prints it on serial at every boot. It is deliberately not derived from the MAC, which is broadcast as the BSSID. Anyone with USB access to the device can read it from serial. A fixed `OTB_AP_PASSWORD` set at build time replaces it: do not publish binaries built with one.
- The web interface has no authentication beyond WiFi access, and uses plain HTTP: anyone on the access point can read the battery data.
- In captive portal mode (default) the built-in DNS server answers every name with the device's IP. This only affects clients joined to the access point. See [docs/web-access.md](docs/web-access.md).
- The serial sniffer (`OTB_SERIAL_SNIFFER=1`) prints every frame, which may include identifying data. Do not share raw logs without removing it.
