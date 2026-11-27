# Getting started

## 1. Install the toolchain

```bash
pip install platformio
```

Or install the PlatformIO extension for VS Code (recommended in `.vscode/extensions.json`).

## 2. Build and flash

```bash
git clone https://github.com/lapht/OpenTeslaBatteryESP.git
cd OpenTeslaBatteryESP

# classic ESP32
pio run -e esp32dev -t upload
pio run -e esp32dev -t uploadfs

# or ESP32-C6
pio run -e esp32c6 -t upload
pio run -e esp32c6 -t uploadfs
```

`upload` flashes the firmware, `uploadfs` flashes the web page from `data/` into LittleFS. Both are needed on first install.

## 3. Check the serial output

```bash
pio device monitor
```

Expected at boot:

```
OpenTeslaBatteryESP - read-only (TWAI listen-only)
[can] listen-only @500k, TX=5 RX=4
[web] SoftAP 'OpenTeslaBattery' up, open http://tesla.lan/ or http://192.168.4.1/ (mDNS: http://tesla.local/)
[can] rx=0 missed=0 buserr=0
```

`rx` grows once the transceiver is connected to an active bus.

## 4. Connect to the car

Wire as described in [wiring.md](wiring.md), plug the harness in, wake the car (open a door or use the app).

## 5. Open the page

Join WiFi `OpenTeslaBattery` (password `teslabattery` by default). Most phones then show a "Sign in to network" prompt that opens the page directly. Otherwise open one of:

- `http://tesla.lan/` (type the `http://` and the trailing `/`, or the browser may search for it)
- `http://192.168.4.1/` (always works)
- `http://tesla.local/` (mDNS)

How this works and what to do when a name does not resolve: [web-access.md](web-access.md).

## Configuration

Settings live in `src/config.h` and can be overridden per environment through `build_flags` in `platformio.ini`:

| Macro | Default | |
|---|---|---|
| `OTB_CAN_TX_PIN` / `OTB_CAN_RX_PIN` | 5 / 4 | Transceiver GPIOs |
| `OTB_AP_SSID` | `OpenTeslaBattery` | SoftAP name |
| `OTB_AP_PASSWORD` | `teslabattery` | **Change it.** Min. 8 characters |
| `OTB_DNS_NAME` | `tesla.lan` | Friendly name served by the built-in DNS. Lowercase |
| `OTB_MDNS_NAME` | `tesla` | mDNS name, reachable as `<name>.local` |
| `OTB_CAPTIVE_PORTAL` | 1 | 1 answers every DNS name and redirects to the page, 0 answers only `OTB_DNS_NAME` |
| `OTB_STALE_MS` | 5000 | Max age of a value used for SOH |
| `OTB_SERIAL_SNIFFER` | 0 | 1 prints every frame to serial |

## Running the tests

```bash
pio test -e native
```

These run on your computer and cover signal extraction, decoders and health math.

## Troubleshooting

| Symptom | Likely cause |
|---|---|
| `TWAI init failed` | Wrong GPIOs for your board |
| `rx=0`, `buserr` growing | CAN-H/CAN-L swapped, wrong bitrate, transceiver not powered |
| Frames received but no 0x292/0x352 | Wrong bus: you are not on Vehicle CAN |
| `tesla.lan` does not open, IP does | Phone sends DNS over mobile data or Private DNS: see [web-access.md](web-access.md#troubleshooting) |
| Page shows `--` everywhere | Car asleep, or `uploadfs` not run (404 on `/`) |
