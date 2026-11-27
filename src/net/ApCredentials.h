#pragma once
#include <Arduino.h>

// SoftAP name and password.
//
// SSID: OTB_AP_SSID if set at build time, otherwise "OpenTeslaBattery-XXXX"
// (last two bytes of the SoftAP MAC).
// Password: OTB_AP_PASSWORD (default "teslabattery", override at build time).
// Fixed on purpose: the user must be able to join from a phone without a PC.
struct ApCredentials {
  String ssid;
  String password;
  bool customPassword = false;  // true if OTB_AP_PASSWORD differs from the default
};

ApCredentials loadApCredentials();
