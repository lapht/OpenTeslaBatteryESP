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

// SoftAP settings. Change the password before using it anywhere public.
#ifndef OTB_AP_SSID
#define OTB_AP_SSID "OpenTeslaBattery"
#endif
#ifndef OTB_AP_PASSWORD
#define OTB_AP_PASSWORD "teslabattery"  // 8-63 chars, WPA2
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

// Print every received frame to serial (sniffer mode, useful to verify the bus).
#ifndef OTB_SERIAL_SNIFFER
#define OTB_SERIAL_SNIFFER 0
#endif
