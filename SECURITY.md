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

- The SoftAP password is fixed, `teslabattery` by default, so you can join from a phone without a PC. That default is public (it is in this repository and in the release binaries), so anyone in WiFi range who knows the project can join and read the page. Set your own with `OTB_AP_PASSWORD` at build time. The device has no write API, so a stranger on the access point can read battery data but cannot change anything.
- The web interface has no authentication beyond WiFi access, and uses plain HTTP: anyone on the access point can read the battery data.
- In captive portal mode (default) the built-in DNS server answers every name with the device's IP. This only affects clients joined to the access point. See [docs/web-access.md](docs/web-access.md).
- The serial sniffer (`OTB_SERIAL_SNIFFER=1`) prints every frame, which may include identifying data. Do not share raw logs without removing it.
