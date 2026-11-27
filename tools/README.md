# Tools

Host-side helper scripts. Python 3.8+, no dependencies.

## can_log_summary.py

Summarises a serial capture made with `OTB_SERIAL_SNIFFER=1`: lists every CAN ID with frame count and rate, and flags missing battery IDs.

```bash
pio device monitor | tee capture.log    # ~30 s with the car awake, Ctrl+C
python3 tools/can_log_summary.py capture.log
```

If 0x292 / 0x352 are missing you are probably not on Vehicle CAN.
