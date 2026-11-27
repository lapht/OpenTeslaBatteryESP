#pragma once
// Build-time configuration. Override via build_flags in platformio.ini.

// CAN transceiver pins (ESP32 GPIO -> SN65HVD230 D/R).
// Defaults are placeholders: confirm against your own wiring.
#ifndef OTB_CAN_TX_PIN
#define OTB_CAN_TX_PIN 5
#endif
#ifndef OTB_CAN_RX_PIN
#define OTB_CAN_RX_PIN 4
#endif

// SoftAP settings.
// SSID: define OTB_AP_SSID to force a fixed name. By default the name is
// OTB_AP_SSID_BASE + "-XXXX" (last two bytes of the MAC), e.g. "OpenTeslaBattery-3FA2".
#ifndef OTB_AP_SSID_BASE
#define OTB_AP_SSID_BASE "OpenTeslaBattery"
#endif
// Password: fixed, so you can join from a phone without reading anything off a PC.
// The default is public (it is in this repo): set your own at build time, e.g.
//   build_flags = ${env.build_flags} '-DOTB_AP_PASSWORD="my-own-secret"'
#define OTB_AP_PASSWORD_DEFAULT "teslabattery"
#ifndef OTB_AP_PASSWORD
#define OTB_AP_PASSWORD OTB_AP_PASSWORD_DEFAULT
#endif
// WiFi.softAP() silently fails with a shorter/longer passphrase: catch it at build time.
static_assert(sizeof(OTB_AP_PASSWORD) - 1 >= 8 && sizeof(OTB_AP_PASSWORD) - 1 <= 63,
              "OTB_AP_PASSWORD must be 8-63 characters (WPA2)");

// Friendly name for the web UI, in addition to http://192.168.4.1/.
// The ESP runs its own DNS server on the SoftAP, so this name resolves on any
// client joined to it. It is not a public domain and does not need to be one.
#ifndef OTB_DNS_NAME
#define OTB_DNS_NAME "tesla.lan"
#endif
// mDNS host name (without ".local"): http://tesla.local/ on clients with mDNS.
#ifndef OTB_MDNS_NAME
#define OTB_MDNS_NAME "tesla"
#endif
// 1 = captive portal: the DNS server answers every name with the ESP's IP and
// unknown hosts are redirected to http://OTB_DNS_NAME/, so phones open the page
// on their own ("Sign in to network"). 0 = answer only OTB_DNS_NAME, no redirect.
#ifndef OTB_CAPTIVE_PORTAL
#define OTB_CAPTIVE_PORTAL 1
#endif

// A value is considered stale if no frame was seen for this long.
#ifndef OTB_STALE_MS
#define OTB_STALE_MS 5000
#endif

// 1 = use the TWAI hardware acceptance filter for the battery IDs while battery
// frames arrive, to keep CPU load low on a busy bus. It opens automatically when
// they stop, so the page can still diagnose a wrong bus. Forced off in sniffer mode.
#ifndef OTB_CAN_HW_FILTER
#define OTB_CAN_HW_FILTER 1
#endif

// Print every received frame to serial (sniffer mode, useful to verify the bus).
#ifndef OTB_SERIAL_SNIFFER
#define OTB_SERIAL_SNIFFER 0
#endif
