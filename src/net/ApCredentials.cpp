#include "ApCredentials.h"

#include <esp_mac.h>

#include "../config.h"
#include "ApCredentialsFormat.h"

ApCredentials loadApCredentials() {
  ApCredentials c;
#ifdef OTB_AP_SSID
  c.ssid = OTB_AP_SSID;
#else
  uint8_t mac[6] = {0};
  esp_read_mac(mac, ESP_MAC_WIFI_SOFTAP);
  char buf[48];
  otb::formatSsid(OTB_AP_SSID_BASE, mac, buf, sizeof(buf));
  c.ssid = buf;
#endif
  c.password = OTB_AP_PASSWORD;
  c.customPassword = c.password != OTB_AP_PASSWORD_DEFAULT;
  return c;
}
