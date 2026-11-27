#include "WebUi.h"

#include <Arduino.h>
#include <ArduinoJson.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <WiFi.h>

static AsyncWebServer server(80);
static WebUi::Snapshot snapshotFn;

static void addValue(JsonObject o, const char* key, const Value& v) {
  if (v.valid) o[key] = v.v; else o[key] = nullptr;
}

bool WebUi::begin(const char* ssid, const char* password, Snapshot snapshot) {
  snapshotFn = snapshot;
  if (!LittleFS.begin(true)) Serial.println("[web] LittleFS mount failed");

  WiFi.mode(WIFI_AP);
  if (!WiFi.softAP(ssid, password)) return false;
  Serial.printf("[web] SoftAP '%s' up, open http://%s/\n", ssid, WiFi.softAPIP().toString().c_str());

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
  server.onNotFound([](AsyncWebServerRequest* req) { req->send(404, "text/plain", "Not found"); });
  server.begin();
  return true;
}
