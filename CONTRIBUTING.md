# Contributing

Thanks for helping. The most valuable contribution right now is **data from real cars**: see [docs/validation.md](docs/validation.md).

## The one hard rule

This firmware is **read-only**. Do not submit code that transmits on the CAN bus in any form (frames, injection, ACKs, "unlocks", config changes). Such PRs will be closed without review.

## Development setup

```bash
pip install platformio
pio test -e native        # unit tests, no hardware
pio run -e esp32dev       # build classic ESP32
pio run -e esp32c6        # build ESP32-C6
```

CI runs all three on every pull request.

## Adding or changing a CAN signal

- Cite the source: DBC file, commit and the exact `SG_` line, in a comment next to the decoder.
- Add a unit test with a synthetic payload.
- Update [docs/can-mapping.md](docs/can-mapping.md) and mark the signal **not validated** unless you include a validation report.
- Never guess a bit position or scale factor. If the source is unclear, open an issue first.

## Code style

- C++17, 2-space indent (see `.editorconfig`).
- Keep decoding and math free of Arduino headers so it stays testable on the host.
- Small, focused PRs. Fill in the PR template.

## Reporting

- Bugs: use the bug report template.
- Values that look wrong: use the car validation template and include raw frames if you can. Remove your VIN.

By contributing you agree that your contributions are licensed under the MIT License and that you follow the [Code of Conduct](CODE_OF_CONDUCT.md).
