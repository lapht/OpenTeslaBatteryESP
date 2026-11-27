#include "ApCredentials.h"

#include <Preferences.h>
#include <esp_mac.h>
#include <esp_random.h>

#include "../config.h"
#include "ApCredentialsFormat.h"

static String makeSsid() {
#ifdef OTB_AP_SSID
  return String(OTB_AP_SSID);
#else
  uint8_t mac[6] = {0};
  esp_read_mac(mac, ESP_MAC_WIFI_SOFTAP);
  char buf[48];
  otb::formatSsid(OTB_AP_SSID_BASE, mac, buf, sizeof(buf));
  return String(buf);
#endif
}

ApCredentials loadApCredentials() {
  ApCredentials c;
  c.ssid = makeSsid();

#ifdef OTB_AP_PASSWORD
  c.password = OTB_AP_PASSWORD;
  c.generated = false;
#else
  Preferences prefs;
  prefs.begin("otb", false);
  String pw = prefs.getString("ap_pw", "");
  if (!otb::isValidPassword(pw.c_str())) {
    uint32_t rnd[otb::kPasswordChars];
    for (auto& r : rnd) r = esp_random();
    char buf[otb::kPasswordBufLen];
    otb::formatPassword(rnd, buf);
    pw = buf;
    if (prefs.putString("ap_pw", pw) == 0) {
      Serial.println("[wifi] WARNING: could not store the password in NVS, it will change at next boot");
    }
  }
  prefs.end();
  c.password = pw;
  c.generated = true;
#endif
  return c;
}
