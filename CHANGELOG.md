# Changelog

Format based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/). Versions follow [SemVer](https://semver.org/).

## [Unreleased]

### Added
- PlatformIO project for ESP32 (classic) and ESP32-C6, Arduino core 3.x.
- TWAI listen-only CAN reader at 500 kbit/s.
- Decoders for 0x292 (beginning-of-life energy, UI SOC) and 0x352 (nominal full pack, remaining, buffer), from model3dbc. Not yet validated on a car.
- SOH and degradation calculation with staleness check.
- WiFi SoftAP, mobile web page, `/api/status` JSON endpoint.
- Friendly URL `http://tesla.lan/` via a DNS server on the SoftAP, captive portal redirect so phones open the page on their own, mDNS `tesla.local`.
- Serial sniffer mode and `tools/can_log_summary.py`.
- Host unit tests, GitHub Actions CI.
- Documentation, issue and PR templates.
