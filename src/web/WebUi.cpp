#include "WebUi.h"

#include <Arduino.h>
#include <ArduinoJson.h>
#include <AsyncTCP.h>
#include <DNSServer.h>
#include <ESPAsyncWebServer.h>
#include <ESPmDNS.h>
#include <LittleFS.h>
#include <WiFi.h>

#include "../config.h"
#include "../net/ApCredentials.h"

static AsyncWebServer server(80);
static DNSServer dns;  // async (AsyncUDP) in Arduino-ESP32 3.x, no loop() call needed
static WebUi::Snapshot snapshotFn;

static void addValue(JsonObject o, const char* key, const Value& v) {
  if (v.valid) o[key] = v.v; else o[key] = nullptr;
}

#if OTB_CAPTIVE_PORTAL
// True if the request was addressed to this device (friendly name, mDNS name or IP).
static bool isOwnHost(AsyncWebServerRequest* req) {
  String host = req->host();
  int colon = host.indexOf(':');
  if (colon >= 0) host = host.substring(0, colon);
  host.toLowerCase();
  if (host.isEmpty()) return true;
  if (host == OTB_DNS_NAME) return true;
  if (host == String(OTB_MDNS_NAME) + ".local") return true;
  return host == WiFi.softAPIP().toString();
}
#endif

static void startNameServices(const IPAddress& ip) {
#if OTB_CAPTIVE_PORTAL
  // Wildcard: every A query gets our IP, which makes phones detect a captive portal.
  dns.setErrorReplyCode(DNSReplyCode::NoError);
  bool ok = dns.start(53, "*", ip);
#else
  dns.setErrorReplyCode(DNSReplyCode::NonExistentDomain);
  bool ok = dns.start(53, OTB_DNS_NAME, ip);
#endif
  if (!ok) Serial.println("[web] DNS server failed to start");

  if (MDNS.begin(OTB_MDNS_NAME)) {
    MDNS.addService("http", "tcp", 80);
  } else {
    Serial.println("[web] mDNS failed to start");
  }
}

bool WebUi::begin(Snapshot snapshot) {
  snapshotFn = snapshot;
  if (!LittleFS.begin(true)) Serial.println("[web] LittleFS mount failed");

  WiFi.mode(WIFI_AP);  // RF on first: esp_random() is a true RNG only from here
  ApCredentials ap = loadApCredentials();
  if (!WiFi.softAP(ap.ssid.c_str(), ap.password.c_str())) {
    Serial.println("[web] SoftAP failed to start");
    return false;
  }
  IPAddress ip = WiFi.softAPIP();
  startNameServices(ip);
  Serial.printf("[wifi] SSID: %s\n", ap.ssid.c_str());
  Serial.printf("[wifi] password: %s%s\n", ap.password.c_str(),
                ap.generated ? "  (unique to this device, kept across updates)" : "  (set at build time)");
  Serial.printf("[web] open http://%s/ or http://%s/ (mDNS: http://%s.local/)\n", OTB_DNS_NAME,
                ip.toString().c_str(), OTB_MDNS_NAME);

  server.on("/api/status", HTTP_GET, [](AsyncWebServerRequest* req) {
    BmsState bms; Health h; CanStats can;
    snapshotFn(bms, h, can);

    JsonDocument doc;
    JsonObject health = doc["health"].to<JsonObject>();
    health["valid"] = h.valid;
    if (h.valid) {
      health["soh_pct"] = h.sohPct;
      health["degradation_pct"] = h.degradationPct;
      health["original_kwh"] = h.originalKWh;
      health["current_kwh"] = h.currentKWh;
    }
    JsonObject raw = doc["bms"].to<JsonObject>();
    addValue(raw, "beginning_of_life_kwh", bms.beginningOfLifeEnergyKWh);
    addValue(raw, "nominal_full_pack_kwh", bms.nominalFullPackEnergyKWh);
    addValue(raw, "nominal_remaining_kwh", bms.nominalEnergyRemainingKWh);
    addValue(raw, "energy_buffer_kwh", bms.energyBufferKWh);
    addValue(raw, "soc_ui_pct", bms.socUiPct);
    JsonObject c = doc["can"].to<JsonObject>();
    c["received"] = can.received;
    c["missed"] = can.missed;
    c["bus_errors"] = can.busErrors;
    doc["uptime_ms"] = millis();

    String out;
    serializeJson(doc, out);
    req->send(200, "application/json", out);
  });

  server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html");
  server.onNotFound([](AsyncWebServerRequest* req) {
#if OTB_CAPTIVE_PORTAL
    // OS connectivity probes (/generate_204, /hotspot-detect.html, /connecttest.txt...)
    // arrive with a foreign Host: redirect them so the phone opens our page.
    if (!isOwnHost(req)) {
      req->redirect("http://" OTB_DNS_NAME "/");
      return;
    }
#endif
    req->send(404, "text/plain", "Not found");
  });
  server.begin();
  return true;
}
