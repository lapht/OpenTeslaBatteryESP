# Security

## Scope

Relevant reports include anything that could make the device **transmit on the CAN bus**, bypass listen-only mode, or expose the web interface beyond the local access point.

## Reporting

Please do not open a public issue. Use GitHub's [private vulnerability reporting](https://github.com/lapht/OpenTeslaBatteryESP/security/advisories/new) for this repository.

## Known limitations

- The SoftAP ships with a default password. Change `OTB_AP_PASSWORD` before use.
- The web interface has no authentication beyond WiFi access.
