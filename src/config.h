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
#define OTB_AP_PASSWORD "teslabattery"  // min. 8 chars, WPA2
#endif

// A value is considered stale if no frame was seen for this long.
#ifndef OTB_STALE_MS
#define OTB_STALE_MS 5000
#endif

// Print every received frame to serial (sniffer mode, useful to verify the bus).
#ifndef OTB_SERIAL_SNIFFER
#define OTB_SERIAL_SNIFFER 0
#endif
