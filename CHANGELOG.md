# Changelog

Format based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/). Versions follow [SemVer](https://semver.org/).

## [Unreleased]

### Added
- PlatformIO project for ESP32 (classic) and ESP32-C6, Arduino core 3.x.
- TWAI listen-only CAN reader at 500 kbit/s.
- Decoders for 0x292 (beginning-of-life energy, UI SOC) and 0x352 (nominal full pack, remaining, buffer), from model3dbc. Not yet validated on a car.
- SOH and degradation calculation with staleness check. SOH is the plain full-pack ratio, energy buffer not subtracted (rationale in `docs/battery-health.md`).
- WiFi SoftAP, mobile web page, `/api/status` JSON endpoint.
- Friendly URL `http://tesla.lan/` via a DNS server on the SoftAP, captive portal redirect so phones open the page on their own, mDNS `tesla.local`.
- Per-device SoftAP: SSID `OpenTeslaBattery-XXXX` from the MAC, random password generated on first boot, stored in NVS and printed on serial (no shared default password).
- Web page shows stale values greyed with their age, and tells apart "device unreachable", "no CAN traffic" and "traffic but no battery frames". `/api/status` adds `bms_age_ms`, `link`, `stale_ms` and frame ages.
- TWAI hardware acceptance filter for the battery IDs, narrowed while battery frames arrive and opened again when they stop (`OTB_CAN_HW_FILTER`).
- Single flashable image per board (`pio run -t flashlayout` + `tools/merge_bin.py`), built in CI; on a `v*` tag CI creates a draft release with the images, `SHA256SUMS.txt` and esptool flash instructions.
- Serial sniffer mode and `tools/can_log_summary.py`.
- Host unit tests, GitHub Actions CI.
- Documentation, issue and PR templates.
