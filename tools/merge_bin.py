#!/usr/bin/env python3
"""Build one flashable image (offset 0x0) from a flash_layout.json.

    pio run -e esp32dev && pio run -e esp32dev -t buildfs && pio run -e esp32dev -t flashlayout
    python3 tools/merge_bin.py .pio/build/esp32dev/flash_layout.json dist/openteslabattery-esp32dev.bin

Needs esptool (`pip install esptool`). The output can be flashed with
`esptool.py write_flash 0x0 <file>` or from the browser with ESP Web Tools.
"""

import json
import os
import subprocess
import sys


def main(argv):
    if len(argv) != 3:
        print(__doc__)
        return 2
    layout_path, out = argv[1], argv[2]
    with open(layout_path) as fh:
        layout = json.load(fh)

    missing = [i["path"] for i in layout["images"] if not os.path.isfile(i["path"])]
    if missing:
        print("Missing images (build firmware and LittleFS first):\n  " + "\n  ".join(missing))
        return 1

    os.makedirs(os.path.dirname(os.path.abspath(out)), exist_ok=True)
    cmd = [sys.executable, "-m", "esptool", "--chip", layout["chip"], "merge_bin", "-o", out,
           "--flash_mode", layout["flash_mode"], "--flash_freq", layout["flash_freq"],
           "--flash_size", layout["flash_size"]]
    for img in layout["images"]:
        cmd += [img["offset"], img["path"]]
    print(" ".join(cmd))
    return subprocess.call(cmd)


if __name__ == "__main__":
    sys.exit(main(sys.argv))
