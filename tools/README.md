# Tools

Host-side helper scripts. Python 3.8+; only `merge_bin.py` needs a package (`esptool`).

## can_log_summary.py

Summarises a serial capture made with `OTB_SERIAL_SNIFFER=1`: lists every CAN ID with frame count and rate, and flags missing battery IDs.

```bash
pio device monitor | tee capture.log    # ~30 s with the car awake, Ctrl+C
python3 tools/can_log_summary.py capture.log
```

If 0x292 / 0x352 are missing you are probably not on Vehicle CAN.

## pio_flash_layout.py and merge_bin.py

Build one flashable image (bootloader + partition table + firmware + LittleFS, offset `0x0`), as attached to releases and used by browser flashing.

- `pio_flash_layout.py` is a PlatformIO extra script: `pio run -e <env> -t flashlayout` writes `.pio/build/<env>/flash_layout.json` (offsets from the partition table and the platform's bootloader images).
- `merge_bin.py` reads that file and calls `esptool merge_bin` (needs `pip install esptool`).

```bash
pio run -e esp32dev && pio run -e esp32dev -t buildfs && pio run -e esp32dev -t flashlayout
python3 tools/merge_bin.py .pio/build/esp32dev/flash_layout.json dist/openteslabattery-esp32dev.bin
```

CI builds these on every run and, on a `v*` tag, attaches them with `SHA256SUMS.txt` to a draft release (marked pre-release when the tag has a suffix, e.g. `v0.1.0-alpha.1`). Publishing it redeploys the site with the new images. Without a terminal: Actions → build → Run workflow on `main` with the tag name; the tag is created when the draft is published.

## social_preview.html

Source of `site/img/social-preview.png` (1280×640), used as the site's `og:image` and as the repository's social preview (uploaded by hand in Settings → General → Social preview). Regenerate it after changing `site/img/dashboard.png`:

```bash
node -e "const {chromium}=require('playwright');(async()=>{const b=await chromium.launch();const p=await b.newPage({viewport:{width:1280,height:640}});await p.goto('file://'+process.cwd()+'/tools/social_preview.html');await p.screenshot({path:'site/img/social-preview.png'});await b.close()})()"
```
