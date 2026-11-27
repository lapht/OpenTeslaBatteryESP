// OpenTeslaBatteryESP - read-only Tesla battery health reader.
// Listens to Vehicle CAN (500 kbit/s) in TWAI listen-only mode, decodes BMS
// frames and serves the results on a web page over a WiFi SoftAP.

#include <Arduino.h>

#include "bms/BmsDecoder.h"
#include "bms/HealthCalculator.h"
#include "can/AcceptanceFilter.h"
#include "can/CanReader.h"
#include "config.h"
#include "web/WebUi.h"

static CanReader can;
static BmsDecoder decoder;
static WebUi web;
static portMUX_TYPE stateLock = portMUX_INITIALIZER_UNLOCKED;

// IDs the firmware decodes; the hardware filter lets (at least) these through.
static const uint32_t kBatteryIds[] = {BmsDecoder::ID_BMS_SOC, BmsDecoder::ID_BMS_ENERGY_STATUS};

#if OTB_CAN_HW_FILTER && !OTB_SERIAL_SNIFFER
// Narrow/open the hardware filter (see FilterPolicy). Runs in the CAN task only.
static void updateFilter(bool batterySeen, uint32_t lastBatteryMs, uint32_t& modeSinceMs) {
  uint32_t now = millis();
  bool want = FilterPolicy::wantFiltered(can.filtered(), batterySeen, now - lastBatteryMs,
                                         now - modeSinceMs);
  if (want == can.filtered()) return;
  bool ok = can.setFiltered(want);
  modeSinceMs = now;
  Serial.printf("[can] hardware filter %s%s\n", can.filtered() ? "on (battery IDs)" : "off (accept all)",
                ok ? "" : " - reinstall failed, using accept-all");
}
#endif

static void canTask(void*) {
  CanFrame f;
  bool batterySeen = false;
  uint32_t lastBatteryMs = 0;
  uint32_t modeSinceMs = millis();
  (void)batterySeen; (void)lastBatteryMs; (void)modeSinceMs;
  for (;;) {
    bool got = can.read(f, 100);
#if OTB_CAN_HW_FILTER && !OTB_SERIAL_SNIFFER
    updateFilter(batterySeen, lastBatteryMs, modeSinceMs);
#endif
    if (!got) continue;
#if OTB_SERIAL_SNIFFER
    Serial.printf("%10lu %03lX [%u]", (unsigned long)f.timestampMs, (unsigned long)f.id, f.dlc);
    for (uint8_t i = 0; i < f.dlc; i++) Serial.printf(" %02X", f.data[i]);
    Serial.println();
#endif
    portENTER_CRITICAL(&stateLock);
    bool battery = decoder.decode(f.id, f.data, f.dlc, f.timestampMs);
    portEXIT_CRITICAL(&stateLock);
    if (battery) {
      batterySeen = true;
      lastBatteryMs = f.timestampMs;
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println("\nOpenTeslaBatteryESP - read-only (TWAI listen-only)");

  if (!can.begin(OTB_CAN_TX_PIN, OTB_CAN_RX_PIN, kBatteryIds, sizeof(kBatteryIds) / sizeof(kBatteryIds[0]))) {
    Serial.println("[can] TWAI init failed");
  } else {
    Serial.printf("[can] listen-only @500k, TX=%d RX=%d\n", OTB_CAN_TX_PIN, OTB_CAN_RX_PIN);
    xTaskCreate(canTask, "can", 4096, nullptr, 5, nullptr);
  }

  web.begin([](BmsState& bms, Health& h, CanStats& stats) {
    portENTER_CRITICAL(&stateLock);
    bms = decoder.state();
    portEXIT_CRITICAL(&stateLock);
    h = computeHealth(bms, millis(), OTB_STALE_MS);
    stats = can.stats();
  });
}

void loop() {
  static uint32_t last = 0;
  if (millis() - last > 5000) {
    last = millis();
    CanStats s = can.stats();
    Serial.printf("[can] rx=%lu missed=%lu buserr=%lu filter=%s\n", (unsigned long)s.received,
                  (unsigned long)s.missed, (unsigned long)s.busErrors, s.filtered ? "on" : "off");
  }
  delay(50);
}
