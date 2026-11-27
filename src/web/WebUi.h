#pragma once
#include <functional>
#include "../can/CanReader.h"
#include "../bms/HealthCalculator.h"

// SoftAP + async HTTP server + DNS (friendly name / captive portal) + mDNS.
//   GET /            -> data/index.html (LittleFS)
//   GET /api/status  -> JSON snapshot (health, raw BMS values, CAN stats)
class WebUi {
 public:
  using Snapshot = std::function<void(BmsState&, Health&, CanStats&)>;
  // SSID and password come from loadApCredentials() (per-device by default).
  bool begin(Snapshot snapshot);
};
