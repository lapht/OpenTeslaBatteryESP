# PlatformIO extra script: `pio run -e <env> -t flashlayout` writes
# .pio/build/<env>/flash_layout.json with everything needed to build a single
# flashable image (bootloader + partition table + app + LittleFS) at offset 0.
# tools/merge_bin.py turns that file into the merged image.
#
# Kept separate from the merge itself so it does not depend on which Python /
# esptool PlatformIO bundles: the merge runs with the system Python + esptool.

import csv
import json
import os

Import("env")  # noqa: F821  (provided by SCons)


def _int(value):
    return int(value.strip(), 0)


def _size(value):
    """Partition size: plain number, or with a K / M suffix (e.g. 1600K, 3M)."""
    v = value.strip().upper()
    for suffix, mult in (("K", 1024), ("M", 1024 * 1024)):
        if v.endswith(suffix):
            return _int(v[:-1]) * mult
    return _int(v)


def _partitions(csv_path):
    """Rows of the partition table with offsets/sizes resolved (blank offsets are auto)."""
    rows = []
    next_offset = 0x9000  # partition table at 0x8000, first partition right after
    with open(csv_path, newline="") as fh:
        for raw in csv.reader(fh):
            if not raw or raw[0].strip().startswith("#"):
                continue
            name, ptype, subtype, offset, size = (c.strip() for c in raw[:5])
            align = 0x10000 if ptype == "app" else 0x1000
            off = _int(offset) if offset else (next_offset + align - 1) // align * align
            sz = _size(size)
            rows.append({"name": name, "type": ptype, "subtype": subtype, "offset": off, "size": sz})
            next_offset = off + sz
    return rows


def _write_layout(target, source, env):  # noqa: ARG001 (SCons signature)
    build_dir = env.subst("$BUILD_DIR")
    board = env.BoardConfig()
    parts = _partitions(env.subst("$PARTITIONS_TABLE_CSV"))

    app = next(p for p in parts if p["type"] == "app")
    fs = next((p for p in parts if p["type"] == "data" and p["subtype"] in ("spiffs", "littlefs")), None)

    images = [[_int(off) if isinstance(off, str) else off, env.subst(path)]
              for off, path in env.get("FLASH_EXTRA_IMAGES", [])]
    images.append([app["offset"], os.path.join(build_dir, env.subst("${PROGNAME}.bin"))])
    if fs:
        images.append([fs["offset"], os.path.join(build_dir, "littlefs.bin")])
    images.sort(key=lambda i: i[0])

    f_flash = str(board.get("build.f_flash", "40000000L")).rstrip("L")
    layout = {
        "chip": board.get("build.mcu", "esp32"),
        "flash_mode": board.get("build.flash_mode", "dio"),
        "flash_freq": "%dm" % (int(f_flash) // 1000000),
        "flash_size": board.get("upload.flash_size", "4MB"),
        "images": [{"offset": "0x%x" % off, "path": path} for off, path in images],
    }
    out = os.path.join(build_dir, "flash_layout.json")
    with open(out, "w") as fh:
        json.dump(layout, fh, indent=2)
    print("Wrote %s" % out)
    for img in layout["images"]:
        print("  %s  %s" % (img["offset"], img["path"]))


env.AddCustomTarget(
    name="flashlayout",
    dependencies=None,
    actions=[_write_layout],
    title="Flash layout",
    description="Write flash_layout.json for tools/merge_bin.py",
)
