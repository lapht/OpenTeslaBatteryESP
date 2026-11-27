#pragma once
#include <Arduino.h>

// SoftAP name and password for this device.
//
// SSID: OTB_AP_SSID if set at build time, otherwise "OpenTeslaBattery-XXXX"
// (last two bytes of the SoftAP MAC).
//
// Password: OTB_AP_PASSWORD if set at build time. Otherwise a random password is
// generated with the hardware RNG on first boot and kept in NVS, so it survives
// firmware and LittleFS updates. It is NOT derived from the MAC: the MAC is
// broadcast as the BSSID, so a MAC-derived password could be computed by anyone
// in radio range. Erasing the flash generates a new one.
struct ApCredentials {
  String ssid;
  String password;
  bool generated = false;  // true if the password is the per-device random one
};

// Call after WiFi.mode(WIFI_AP): the RF subsystem must be on for esp_random()
// to be a true random source.
ApCredentials loadApCredentials();
