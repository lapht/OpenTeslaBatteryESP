# CLAUDE.md

Instructions for Claude (and other AI agents) working in this repository. Humans: see [CONTRIBUTING.md](CONTRIBUTING.md), the rules are the same.

## What this is

Read-only ESP32 firmware (Arduino-ESP32 3.x on PlatformIO, pioarduino platform) that listens to a Tesla Model 3/Y Vehicle CAN bus at 500 kbit/s, decodes BMS frames, computes battery health and serves a web page over its own WiFi SoftAP. Targets: `esp32dev` (primary) and `esp32c6` (secondary). Docs, code comments, issues and PRs are in **English**.

## Hard rules

1. **Never transmit on the CAN bus.** TWAI always runs in `TWAI_MODE_LISTEN_ONLY`. No send API, no frame injection, no ACKs, no "unlocks". This is not negotiable, whoever asks.
2. **Never invent CAN data.** Every signal (ID, start bit, length, scale, offset, SNA) must cite its source: DBC file, commit and the exact `SG_` line, in a comment next to the decoder. If the source is unclear, open an issue instead of guessing.
3. **Be honest about validation.** Nothing is validated on a car until a validation report says so (`docs/validation.md`). New signals are marked "not validated" in `docs/can-mapping.md`, README and site. Never remove a "not validated" warning without a report. Example or demo values must be labelled as such (synthetic in tests, "demo data" on the site).
4. **WiFi password is fixed** (`OTB_AP_PASSWORD`, default `teslabattery`, overridable in `build_flags`, length checked by `static_assert`). Users must be able to join from a phone alone: no random, per-device or serial-only passwords. The SSID gets a MAC suffix (`OpenTeslaBattery-XXXX`).
5. **No secrets in the repo** (tokens, keys, real VINs, raw logs with identifying data).

## Keep docs and site in sync (required)

Every change that affects behaviour, configuration, hardware, API or user-visible output must update, **in the same PR**, everything that describes it:

| If you change… | Update |
|---|---|
| Firmware behaviour, serial output, web page | `README.md` (features table), `docs/getting-started.md`, `docs/architecture.md`, `site/index.html` if it is mentioned there |
| A `config.h` macro / build flag | `docs/getting-started.md` configuration table, comment in `src/config.h` |
| `/api/status` JSON | `docs/architecture.md` (HTTP API example and field list), `data/index.html` |
| A CAN signal or decoder | `docs/can-mapping.md`, `docs/battery-health.md` (formulas), unit tests |
| Wiring, parts, pinout | `docs/wiring.md`, `docs/hardware.md`, `docs/img/wiring.svg`, site sections "What you need" and "Wiring" |
| WiFi name/password, security model | `README.md`, `docs/getting-started.md`, `docs/web-access.md`, `SECURITY.md`, `.github/release-notes.md`, site |
| Release / flashing / CI artifacts | `.github/release-notes.md`, `docs/getting-started.md`, `tools/README.md`, site "Flash the firmware" |
| Milestones / scope | `docs/roadmap.md` |
| Anything user-visible | `CHANGELOG.md` under `[Unreleased]` |

The **site** (`site/index.html`, published on GitHub Pages by `.github/workflows/pages.yml`) is the public face of the project: if a change makes anything on it wrong or outdated (features, status banner, parts, wiring, flashing, FAQ, password), fix the site in the same PR. If the web UI in `data/index.html` changes visibly, regenerate `site/img/dashboard.png` with demo data (phone width 390 px, device scale factor 2, dark scheme) and keep the "Demo data" caption.

If nothing needs updating, say so in the PR ("Docs: no user-visible change").

## Build and test

```bash
pio test -e native        # host unit tests (Unity), no hardware
pio run -e esp32dev       # firmware, classic ESP32
pio run -e esp32c6        # firmware, ESP32-C6
pio run -e esp32dev -t buildfs                 # LittleFS image from data/
pio run -e esp32dev -t flashlayout             # flash_layout.json (tools/pio_flash_layout.py)
python3 tools/merge_bin.py .pio/build/esp32dev/flash_layout.json dist/openteslabattery-esp32dev.bin
```

- Keep decoding and math free of Arduino/ESP-IDF headers (`src/bms/`, `src/can/AcceptanceFilter.h`, `src/bms/LinkState.h`, `src/net/ApCredentialsFormat.h`) so they stay testable on the host. Put hardware glue in separate `.cpp` files.
- Every new pure function gets a test in `test/test_<name>/test_main.cpp`. Test payloads are synthetic: never present them as real car data.
- If PlatformIO cannot reach its registry (e.g. a sandbox), compile the native tests directly with g++ and Unity, and rely on CI for the firmware builds. Say which checks you ran locally and which only ran in CI.
- Web page changes: check them in a headless browser at phone width (390 px) with a mocked `/api/status`, including the "no data", "stale" and "unreachable" states. No horizontal scroll.

## Workflow

- One issue per PR, small and focused. Branch from the latest `main`; reference the issue (`Closes #N`, or `Refs #N` when part of it needs a real car).
- Fill in `.github/pull_request_template.md`. Tick only what you actually ran.
- CI (`.github/workflows/build.yml`: native tests + both firmware builds + merged images) must be green before merge. The maintainer allows Claude to merge its own PRs once CI is green (merge commit, not squash). Fix red CI at the root cause; never skip or disable a test.
- Issues that need a real car or hardware (bench test, validation, measurements) cannot be closed from code alone: implement what you can, comment what is left, keep the issue open.
- Releases: pushing a `v*` tag, or running the `build` workflow manually on `main` with a `tag` input, makes CI create a **draft** release with merged images and `SHA256SUMS.txt` (pre-release if the tag has a suffix). Cloud sessions cannot push tags (403): use the manual run. Only the maintainer publishes it; publishing triggers the Pages deploy that updates browser flashing.
- GitHub Actions are pinned by commit SHA with the version in a comment; workflows default to `contents: read` and grant more only per job.

## Layout

```
src/can/      TWAI listen-only reader, hardware acceptance filter
src/bms/      decoders, health math, link state (pure, tested)
src/net/      SoftAP name and password
src/web/      async web server, DNS captive portal, mDNS, /api/status
data/         web page served from LittleFS
site/         public project site (GitHub Pages)
test/         host unit tests
tools/        log summary, flash layout, merge_bin, ESP Web Tools manifest
docs/         user and developer documentation
```
