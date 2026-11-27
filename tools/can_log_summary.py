#!/usr/bin/env python3
"""Summarise a serial sniffer log (OTB_SERIAL_SNIFFER=1).

Prints every CAN ID seen with frame count and approximate rate, and flags the
battery IDs this project needs. If 0x292 / 0x352 are missing you are most
likely on the wrong bus or the wrong pins.

Usage: python3 tools/can_log_summary.py capture.log
"""
import re
import sys
from collections import defaultdict

LINE = re.compile(r"^\s*(\d+)\s+([0-9A-Fa-f]{3})\s+\[(\d)\]((?:\s+[0-9A-Fa-f]{2})*)\s*$")
BATTERY_IDS = {
    0x132: "HV battery voltage/current",
    0x292: "BMS SOC + beginning-of-life energy",
    0x332: "Brick min/max voltage and temperature",
    0x33A: "UI range / rated Wh per mile",
    0x352: "BMS energy status (nominal full pack)",
    0x3D2: "Lifetime charge/discharge",
    0x401: "Brick voltages",
}


def main(path: str) -> int:
    counts = defaultdict(int)
    first, last = {}, {}
    with open(path, encoding="utf-8", errors="replace") as f:
        for line in f:
            m = LINE.match(line)
            if not m:
                continue
            ts, can_id = int(m.group(1)), int(m.group(2), 16)
            counts[can_id] += 1
            first.setdefault(can_id, ts)
            last[can_id] = ts

    if not counts:
        print("No frames found. Is OTB_SERIAL_SNIFFER enabled?")
        return 1

    print(f"{'ID':>5}  {'frames':>7}  {'Hz':>6}  note")
    for can_id in sorted(counts):
        span = (last[can_id] - first[can_id]) / 1000
        hz = (counts[can_id] - 1) / span if span > 0 else 0
        note = BATTERY_IDS.get(can_id, "")
        print(f"0x{can_id:03X}  {counts[can_id]:7d}  {hz:6.1f}  {note}")

    missing = [f"0x{i:03X}" for i in BATTERY_IDS if i not in counts]
    if missing:
        print(f"\nMissing battery IDs: {', '.join(missing)}")
        if 0x292 not in counts or 0x352 not in counts:
            print("0x292/0x352 missing: check that you are on Vehicle CAN (see docs/wiring.md).")
    return 0


if __name__ == "__main__":
    if len(sys.argv) != 2:
        print(__doc__)
        sys.exit(2)
    sys.exit(main(sys.argv[1]))
