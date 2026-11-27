#!/usr/bin/env python3
"""Write an ESP Web Tools manifest for the merged release images.

    python3 tools/make_manifest.py <dir-with-bins> <version>

Expects openteslabattery-esp32dev.bin and/or openteslabattery-esp32c6.bin in
<dir>; writes <dir>/manifest.json. Each image is a full flash image at 0x0.
"""

import json
import os
import sys

BUILDS = [
    ("ESP32", "openteslabattery-esp32dev.bin"),
    ("ESP32-C6", "openteslabattery-esp32c6.bin"),
]


def main(argv):
    if len(argv) != 3:
        print(__doc__)
        return 2
    out_dir, version = argv[1], argv[2]
    builds = [
        {"chipFamily": chip, "parts": [{"path": name, "offset": 0}]}
        for chip, name in BUILDS
        if os.path.isfile(os.path.join(out_dir, name))
    ]
    if not builds:
        print("No release images found in " + out_dir)
        return 1
    manifest = {
        "name": "OpenTeslaBatteryESP",
        "version": version,
        "new_install_prompt_erase": False,  # the merged image overwrites the whole flash anyway
        "new_install_improv_wait_time": 0,  # firmware does not speak Improv
        "builds": builds,
    }
    with open(os.path.join(out_dir, "manifest.json"), "w") as fh:
        json.dump(manifest, fh, indent=2)
    print(json.dumps(manifest, indent=2))
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
