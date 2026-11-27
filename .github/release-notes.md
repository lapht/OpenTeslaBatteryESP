> **Read-only, community reverse-engineered data. Not Tesla's official battery test. Not affiliated with Tesla, Inc.**
> Check the README for which signals have been validated on a real car.

## Files

| File | Board |
|---|---|
| `openteslabattery-esp32dev.bin` | ESP32 classic (DevKitC / WROOM-32) |
| `openteslabattery-esp32c6.bin` | ESP32-C6 DevKitC-1 |
| `SHA256SUMS.txt` | checksums |

Each `.bin` is a complete image (bootloader, partition table, firmware, web page) to flash at offset `0x0`.

## Flash

```bash
sha256sum -c SHA256SUMS.txt --ignore-missing
pip install esptool
# ESP32 classic
esptool.py --chip esp32 --baud 460800 write_flash 0x0 openteslabattery-esp32dev.bin
# ESP32-C6
esptool.py --chip esp32c6 --baud 460800 write_flash 0x0 openteslabattery-esp32c6.bin
```

Add `--port /dev/ttyUSB0` (Linux), `/dev/cu.usbserial-*` (macOS) or `COM3` (Windows) if esptool does not find the board. Then join the WiFi `OpenTeslaBattery-XXXX` with password `teslabattery`. That default is public: to use your own, build from source with `OTB_AP_PASSWORD`.

Changes: see [CHANGELOG.md](https://github.com/lapht/OpenTeslaBatteryESP/blob/main/CHANGELOG.md).
